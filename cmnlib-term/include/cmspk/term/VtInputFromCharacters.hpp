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
************************************************/
class VtInputFromCharacters {
  public:
    // feeding
    bool canAppend() { return !data; }
    std::expected<void, VtInputFromCharactersError> append(char8_t character) {
        if (data) {
            return std::unexpected(VtInputFromCharactersError::CANNOT_ACCEPT_ANY_NEW_CHARACTER);
        }
        data = VtInputUnknown(character);
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
};
// ================[ END OF CODE ]================
}  // namespace cmspk::term
#endif
