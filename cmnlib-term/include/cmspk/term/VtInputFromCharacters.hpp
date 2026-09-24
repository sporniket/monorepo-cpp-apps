// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 David SPORN
// ---
// This is part of **term library by sporniket**.
// A collection of utilities for writing terminal-hosted applications.
// ---

#ifndef __CMSPK__TERM__VTINPUTFROMCHARACTERS_HPP__
#define __CMSPK__TERM__VTINPUTFROMCHARACTERS_HPP__

// standard libs
#include <deque>
#include <expected>
#include <optional>
#include <string>
#include <vector>

// project
#include "cmspk/term/VtInput.hpp"

namespace cmspk::term {
// ================[ CODE BEGINS ]================
// Enums of errors
/**
 * Identifiable causes of error.
 */
enum class VtInputFromCharactersError { CANNOT_ACCEPT_ANY_NEW_CHARACTER };

/**
 * States for the internal finite state machine.
 *
 * The goal is to be able to match a message generally defined like this :
 *
 * ```
 * digit:= one_of('0', '1', '2', '3', '4', '5', '6', '7', '8', '9');
 * number:= @digit({.minimum = 1, .maximum = 5});
 * command:= one_of('A'..'Z','~'); // to expand
 * message:= append('\x1b[',optional_of(@number(),optional_of(';', @number())), @command());
 * ```
 */
enum class VtInputSequenceParserState {
    /**
     * Initial state, stay at this state until getting ASCII 27 ('\x1b').
     */
    WAITING_FOR_ESCAPE,
    /**
     * Stay at this state until getting '['.
     */
    WAITING_FOR_CSI,
    /**
     * Stay at this state until getting a digit or a command.
     */
    WAITING_FOR_FIRST_NUMBER_OR_COMMAND,
    /**
     * Stay at this state until getting a ';' or a command.
     */
    WAITING_FOR_NUMBER_SEPARATOR_OR_COMMAND,
    /**
     * Stay at this state until getting a digit.
     */
    WAITING_FOR_SECOND_NUMBER,
    /**
     * Stay at this state until getting command.
     */
    WAITING_FOR_COMMAND,
    /**
     * Final state, the state machine MUST be reset.
     */
    ITS_A_MATCH
};

/**
 * Parser context to match a VT input sequence.
 *
 * A general form of VT input sequence is `'\x1b['+[number + [';' + number]]+command`.
 */
struct VtInputSequenceParserContext {
    VtInputSequenceParserState state = VtInputSequenceParserState::WAITING_FOR_ESCAPE;
    std::basic_string<char8_t> rawAccumulator;
    std::optional<char8_t> command = std::nullopt;
    std::optional<std::basic_string<char8_t>> firstNumber = std::nullopt;
    std::optional<std::basic_string<char8_t>> secondNumber = std::nullopt;

    void reset() {
        state = VtInputSequenceParserState::WAITING_FOR_ESCAPE;
        rawAccumulator.clear();
        command = std::nullopt;
        firstNumber = std::nullopt;
        secondNumber = std::nullopt;
    }

    void initFirstNumber() {
        firstNumber = std::basic_string<char8_t>();
        firstNumber->reserve(5);
    }

    void initSecondNumber() {
        secondNumber = std::basic_string<char8_t>();
        secondNumber->reserve(5);
    }
};

/************************************************
Converts a sequence of characters (`char8_t`) into a sequence of virtual terminal inputs.

Typical application :

```cpp
// GIVEN
VtInputFromCharacters converter ;
converter.reset() ;
static_assert(converter.canGetData() == false, "converter.canPop() should be false after reset")
static_assert(converter.canAppend() == false, "converter.canPush() should be true after reset")

// some inputs only need a single char
converter.append('H') ; // converter.canPush() is false, converter.canPop() is true
if (converter.canGetData()) {
  std::optional<VtInput> result = converter.getData() ; // is present and contains the char8_t 'H'
}

// some input requires more
converter.append(27); // converter.canPop() is false
converter.append('['); // converter.canPop() is false
converter.append('A'); // converter.canPop() is true
if (converter.canGetData()) {
  std::optional<VtInput> result = converter.getData() ; // is present and contains a VtInputKey VtInputKey::ARROW_UP
}

// It can be interrupted, in this case multi-character sequence matching is interrupted
converter.append(27); // converter.canPop() is false
converter.append('['); // converter.canPop() is false
converter.abort(); // converter.canPop() is true
if (converter.canGetData()) {
  std::optional<VtInput> result = converter.getData() ; // is present and contains a VtInputKey VtInputKey::ESCAPE
  std::optional<VtInput> result2 = converter.getData() ; // is present and contains a char8_t '['
}
```

## Requirements

* * [Specfication guidelines](../../README--specification-guidelines.md)
* * cmspk::term::VtInput


## Behaviours


> _BEGIN extracted from `TestRunner--VtInputFromCharacters.cpp`_

<hr>

### VtInputFromCharacters, should_return_an_unknown_virtual_terminal_input_on_reading_an_octet_that_is_not_recognizable ###

> This is the fall-back behaviour. The list [28,29,30,31] is what remains unknown when all other behaviours have been implemented.

__for__ any character _c_ in `[28,29,30,31]`

__given__ VtInputFromCharacters has been reset

__when__ VtInputFromCharacters is fed with _c_

__then__ VtInputFromCharacters does not accept characters anymore

__then__ VtInputFromCharacters does have data

__then__ the VtInputFromCharacters will return a `std::variant` containing a `VtInputUnkown` with _c_ as `rawValue`.

__then__ VtInputFromCharacters does not have data

<hr>

### VtInputFromCharacters, should_return_printable_characters_on_reading_an_octet_with_value_in_range_32_to_256_excluding_127 ###

__for__ any character _c_ in `range(32,256)` excluding 127

__given__ VtInputFromCharacters has been reset

__when__ VtInputFromCharacters is fed with _c_

__then__ VtInputFromCharacters does not accept characters anymore

__then__ VtInputFromCharacters does have data

__then__ the VtInputFromCharacters will return a `std::variant` containing _c_.

__then__ VtInputFromCharacters does not have data

<hr>

### VtInputFromCharacters, should_return_keys_on_reading_an_octet_with_value_127_or_in_range_0_to_27 ###

__for__ any character _c_ in [0,..,26,127]

__given__ VtInputFromCharacters has been reset

__when__ VtInputFromCharacters is fed with _c_

__then__ VtInputFromCharacters does not accept characters anymore

__then__ VtInputFromCharacters does have data

__then__ the VtInputFromCharacters will return a `std::variant` containing the VtInputKey corresponding to _c_.

__then__ VtInputFromCharacters does not have data

<hr>

> _END extracted from `TestRunner--VtInputFromCharacters.cpp`_

************************************************/
class VtInputFromCharacters {
  public:
    // feeding
    bool canAppend() { return data.empty(); }
    std::expected<void, VtInputFromCharactersError> append(char8_t character) {
        if (!data.empty()) {
            return std::unexpected(VtInputFromCharactersError::CANNOT_ACCEPT_ANY_NEW_CHARACTER);
        }
        // TODO either it is processed by the parser...
        switch (sequenceParserContext.state) {
            case VtInputSequenceParserState::WAITING_FOR_ESCAPE:
                if (character == 27) {
                    char8_t buffer[1];
                    buffer[0] = character;
                    sequenceParserContext.rawAccumulator.append(buffer);
                    sequenceParserContext.state = VtInputSequenceParserState::WAITING_FOR_CSI;
                    return ok();
                }
                break;
            case VtInputSequenceParserState::WAITING_FOR_CSI:
                return ok();
            case VtInputSequenceParserState::WAITING_FOR_FIRST_NUMBER_OR_COMMAND:
                return ok();
            case VtInputSequenceParserState::WAITING_FOR_NUMBER_SEPARATOR_OR_COMMAND:
                return ok();
            case VtInputSequenceParserState::WAITING_FOR_SECOND_NUMBER:
                return ok();
            case VtInputSequenceParserState::WAITING_FOR_COMMAND:
                return ok();
            case VtInputSequenceParserState::ITS_A_MATCH:
                return ok();
        }
        // or by the default mapping
        data.push_back(single_octet_map.at((size_t)character));
        return ok();
    }

    // getting data
    bool canGetData() { return !data.empty(); }
    std::optional<VtInput> getData() {
        if (data.empty()) {
            return std::nullopt;
        }
        std::optional<VtInput> result(data.front());
        data.pop_front();
        return result;
    }

    // maintenance
    void abort() {}
    void reset() {
        data.clear();
        sequenceParserContext.reset();
    }

  private:
    std::expected<void, VtInputFromCharactersError> ok() { return std::expected<void, VtInputFromCharactersError>(); }

    std::deque<VtInput> data;
    VtInputSequenceParserContext sequenceParserContext;
    const std::vector<VtInput> single_octet_map{VtInputKey::CTRL_SPACE,  // 0
                                                VtInputKey::CTRL_A,
                                                VtInputKey::CTRL_B,
                                                VtInputKey::CTRL_C,
                                                VtInputKey::CTRL_D,
                                                VtInputKey::CTRL_E,
                                                VtInputKey::CTRL_F,  // 6
                                                VtInputKey::CTRL_G,
                                                VtInputKey::CTRL_H,
                                                VtInputKey::HTAB,
                                                VtInputKey::CTRL_J,
                                                VtInputKey::CTRL_K,
                                                VtInputKey::CTRL_L,
                                                VtInputKey::RETURN,  // 13
                                                VtInputKey::CTRL_N,
                                                VtInputKey::CTRL_O,
                                                VtInputKey::CTRL_P,
                                                VtInputKey::CTRL_Q,
                                                VtInputKey::CTRL_R,
                                                VtInputKey::CTRL_S,
                                                VtInputKey::CTRL_T,  // 20
                                                VtInputKey::CTRL_U,
                                                VtInputKey::CTRL_V,
                                                VtInputKey::CTRL_W,
                                                VtInputKey::CTRL_X,
                                                VtInputKey::CTRL_Y,
                                                VtInputKey::CTRL_Z,
                                                VtInputKey::ESCAPE,  // 27
                                                VtInputUnknown(28),
                                                VtInputUnknown(29),
                                                VtInputUnknown(30),
                                                VtInputUnknown(31),
                                                (char8_t)32,
                                                (char8_t)33,
                                                (char8_t)34,
                                                (char8_t)35,
                                                (char8_t)36,
                                                (char8_t)37,
                                                (char8_t)38,
                                                (char8_t)39,
                                                (char8_t)40,
                                                (char8_t)41,
                                                (char8_t)42,
                                                (char8_t)43,
                                                (char8_t)44,
                                                (char8_t)45,
                                                (char8_t)46,
                                                (char8_t)47,
                                                (char8_t)48,
                                                (char8_t)49,
                                                (char8_t)50,
                                                (char8_t)51,
                                                (char8_t)52,
                                                (char8_t)53,
                                                (char8_t)54,
                                                (char8_t)55,
                                                (char8_t)56,
                                                (char8_t)57,
                                                (char8_t)58,
                                                (char8_t)59,
                                                (char8_t)60,
                                                (char8_t)61,
                                                (char8_t)62,
                                                (char8_t)63,
                                                (char8_t)64,
                                                (char8_t)65,
                                                (char8_t)66,
                                                (char8_t)67,
                                                (char8_t)68,
                                                (char8_t)69,
                                                (char8_t)70,
                                                (char8_t)71,
                                                (char8_t)72,
                                                (char8_t)73,
                                                (char8_t)74,
                                                (char8_t)75,
                                                (char8_t)76,
                                                (char8_t)77,
                                                (char8_t)78,
                                                (char8_t)79,
                                                (char8_t)80,
                                                (char8_t)81,
                                                (char8_t)82,
                                                (char8_t)83,
                                                (char8_t)84,
                                                (char8_t)85,
                                                (char8_t)86,
                                                (char8_t)87,
                                                (char8_t)88,
                                                (char8_t)89,
                                                (char8_t)90,
                                                (char8_t)91,
                                                (char8_t)92,
                                                (char8_t)93,
                                                (char8_t)94,
                                                (char8_t)95,
                                                (char8_t)96,
                                                (char8_t)97,
                                                (char8_t)98,
                                                (char8_t)99,
                                                (char8_t)100,
                                                (char8_t)101,
                                                (char8_t)102,
                                                (char8_t)103,
                                                (char8_t)104,
                                                (char8_t)105,
                                                (char8_t)106,
                                                (char8_t)107,
                                                (char8_t)108,
                                                (char8_t)109,
                                                (char8_t)110,
                                                (char8_t)111,
                                                (char8_t)112,
                                                (char8_t)113,
                                                (char8_t)114,
                                                (char8_t)115,
                                                (char8_t)116,
                                                (char8_t)117,
                                                (char8_t)118,
                                                (char8_t)119,
                                                (char8_t)120,
                                                (char8_t)121,
                                                (char8_t)122,
                                                (char8_t)123,
                                                (char8_t)124,
                                                (char8_t)125,
                                                (char8_t)126,
                                                VtInputKey::BACKSPACE,
                                                (char8_t)128,
                                                (char8_t)129,
                                                (char8_t)130,
                                                (char8_t)131,
                                                (char8_t)132,
                                                (char8_t)133,
                                                (char8_t)134,
                                                (char8_t)135,
                                                (char8_t)136,
                                                (char8_t)137,
                                                (char8_t)138,
                                                (char8_t)139,
                                                (char8_t)140,
                                                (char8_t)141,
                                                (char8_t)142,
                                                (char8_t)143,
                                                (char8_t)144,
                                                (char8_t)145,
                                                (char8_t)146,
                                                (char8_t)147,
                                                (char8_t)148,
                                                (char8_t)149,
                                                (char8_t)150,
                                                (char8_t)151,
                                                (char8_t)152,
                                                (char8_t)153,
                                                (char8_t)154,
                                                (char8_t)155,
                                                (char8_t)156,
                                                (char8_t)157,
                                                (char8_t)158,
                                                (char8_t)159,
                                                (char8_t)160,
                                                (char8_t)161,
                                                (char8_t)162,
                                                (char8_t)163,
                                                (char8_t)164,
                                                (char8_t)165,
                                                (char8_t)166,
                                                (char8_t)167,
                                                (char8_t)168,
                                                (char8_t)169,
                                                (char8_t)170,
                                                (char8_t)171,
                                                (char8_t)172,
                                                (char8_t)173,
                                                (char8_t)174,
                                                (char8_t)175,
                                                (char8_t)176,
                                                (char8_t)177,
                                                (char8_t)178,
                                                (char8_t)179,
                                                (char8_t)180,
                                                (char8_t)181,
                                                (char8_t)182,
                                                (char8_t)183,
                                                (char8_t)184,
                                                (char8_t)185,
                                                (char8_t)186,
                                                (char8_t)187,
                                                (char8_t)188,
                                                (char8_t)189,
                                                (char8_t)190,
                                                (char8_t)191,
                                                (char8_t)192,
                                                (char8_t)193,
                                                (char8_t)194,
                                                (char8_t)195,
                                                (char8_t)196,
                                                (char8_t)197,
                                                (char8_t)198,
                                                (char8_t)199,
                                                (char8_t)200,
                                                (char8_t)201,
                                                (char8_t)202,
                                                (char8_t)203,
                                                (char8_t)204,
                                                (char8_t)205,
                                                (char8_t)206,
                                                (char8_t)207,
                                                (char8_t)208,
                                                (char8_t)209,
                                                (char8_t)210,
                                                (char8_t)211,
                                                (char8_t)212,
                                                (char8_t)213,
                                                (char8_t)214,
                                                (char8_t)215,
                                                (char8_t)216,
                                                (char8_t)217,
                                                (char8_t)218,
                                                (char8_t)219,
                                                (char8_t)220,
                                                (char8_t)221,
                                                (char8_t)222,
                                                (char8_t)223,
                                                (char8_t)224,
                                                (char8_t)225,
                                                (char8_t)226,
                                                (char8_t)227,
                                                (char8_t)228,
                                                (char8_t)229,
                                                (char8_t)230,
                                                (char8_t)231,
                                                (char8_t)232,
                                                (char8_t)233,
                                                (char8_t)234,
                                                (char8_t)235,
                                                (char8_t)236,
                                                (char8_t)237,
                                                (char8_t)238,
                                                (char8_t)239,
                                                (char8_t)240,
                                                (char8_t)241,
                                                (char8_t)242,
                                                (char8_t)243,
                                                (char8_t)244,
                                                (char8_t)245,
                                                (char8_t)246,
                                                (char8_t)247,
                                                (char8_t)248,
                                                (char8_t)249,
                                                (char8_t)250,
                                                (char8_t)251,
                                                (char8_t)252,
                                                (char8_t)253,
                                                (char8_t)254,
                                                (char8_t)255

    };
};
// ================[ END OF CODE ]================
}  // namespace cmspk::term
#endif
