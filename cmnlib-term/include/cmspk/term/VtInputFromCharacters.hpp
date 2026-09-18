// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 David SPORN
// ---
// This is part of **term library by sporniket**.
// A collection of utilities for writing terminal-hosted applications.
// ---

#ifndef __CMSPK__TERM__VTINPUTFROMCHARACTERS_HPP__
#define __CMSPK__TERM__VTINPUTFROMCHARACTERS_HPP__

// standard libs
#include <expected>
#include <optional>
#include <vector>

// project
#include "cmspk/term/VtInput.hpp"

namespace cmspk::term {
// ================[ CODE BEGINS ]================
// Enums of errors
enum class VtInputFromCharactersError { CANNOT_ACCEPT_ANY_NEW_CHARACTER };

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
    bool canAppend() { return !data; }
    std::expected<void, VtInputFromCharactersError> append(char8_t character) {
        if (data) {
            return std::unexpected(VtInputFromCharactersError::CANNOT_ACCEPT_ANY_NEW_CHARACTER);
        }
        if (character == 127) {
            data = VtInputKey::BACKSPACE;
        } else if (character < 27) {
            data = keys_of_range_0_27.at(character);
        } else if (character >= 32) {
            data = character;
        } else {
            data = VtInputUnknown(character);
        }
        return std::expected<void, VtInputFromCharactersError>();
    }

    // getting data
    bool canGetData() { return !!data; }
    std::optional<VtInput> getData() {
        std::optional<VtInput> result(data);
        data = std::nullopt;
        return result;
    }

    // maintenance
    void abort() {}
    void reset() { data = std::nullopt; }

  private:
    std::optional<VtInput> data = std::nullopt;
    const std::vector<VtInputKey> keys_of_range_0_27{
        VtInputKey::CTRL_SPACE, VtInputKey::CTRL_A, VtInputKey::CTRL_B, VtInputKey::CTRL_C, VtInputKey::CTRL_D, VtInputKey::CTRL_E, VtInputKey::CTRL_F,
        VtInputKey::CTRL_G,     VtInputKey::CTRL_H, VtInputKey::HTAB,   VtInputKey::CTRL_J, VtInputKey::CTRL_K, VtInputKey::CTRL_L, VtInputKey::RETURN,
        VtInputKey::CTRL_N,     VtInputKey::CTRL_O, VtInputKey::CTRL_P, VtInputKey::CTRL_Q, VtInputKey::CTRL_R, VtInputKey::CTRL_S, VtInputKey::CTRL_T,
        VtInputKey::CTRL_U,     VtInputKey::CTRL_V, VtInputKey::CTRL_W, VtInputKey::CTRL_X, VtInputKey::CTRL_Y, VtInputKey::CTRL_Z};
};
// ================[ END OF CODE ]================
}  // namespace cmspk::term
#endif
