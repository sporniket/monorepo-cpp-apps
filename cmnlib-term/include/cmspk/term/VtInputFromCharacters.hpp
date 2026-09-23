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
    std::optional<char8_t> command = std::nullopt;
    std::optional<std::basic_string<char8_t>> firstNumber = std::nullopt;
    std::optional<std::basic_string<char8_t>> secondNumber = std::nullopt;

    void reset() {
        state = VtInputSequenceParserState::WAITING_FOR_ESCAPE;
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
        if (character == 127) {
            data.push_back(VtInputKey::BACKSPACE);
        } else if (character < 27) {
            data.push_back(keys_of_range_0_27.at(character));
        } else if (character >= 32) {
            data.push_back(character);
        } else {
            data.push_back(VtInputUnknown(character));
        }
        return std::expected<void, VtInputFromCharactersError>();
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
    void reset() { data.clear(); }

  private:
    std::deque<VtInput> data;
    VtInputSequenceParserContext sequenceParserContext;
    const std::vector<VtInputKey> keys_of_range_0_27{
        VtInputKey::CTRL_SPACE, VtInputKey::CTRL_A, VtInputKey::CTRL_B, VtInputKey::CTRL_C, VtInputKey::CTRL_D, VtInputKey::CTRL_E, VtInputKey::CTRL_F,
        VtInputKey::CTRL_G,     VtInputKey::CTRL_H, VtInputKey::HTAB,   VtInputKey::CTRL_J, VtInputKey::CTRL_K, VtInputKey::CTRL_L, VtInputKey::RETURN,
        VtInputKey::CTRL_N,     VtInputKey::CTRL_O, VtInputKey::CTRL_P, VtInputKey::CTRL_Q, VtInputKey::CTRL_R, VtInputKey::CTRL_S, VtInputKey::CTRL_T,
        VtInputKey::CTRL_U,     VtInputKey::CTRL_V, VtInputKey::CTRL_W, VtInputKey::CTRL_X, VtInputKey::CTRL_Y, VtInputKey::CTRL_Z};
};
// ================[ END OF CODE ]================
}  // namespace cmspk::term
#endif
