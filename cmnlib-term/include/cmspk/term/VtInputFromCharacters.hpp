// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 David SPORN
// ---
// This is part of **term library by sporniket**.
// A collection of utilities for writing terminal-hosted applications.
// ---

#ifndef __CMSPK__TERM__VTINPUTFROMCHARACTERS_HPP__
#define __CMSPK__TERM__VTINPUTFROMCHARACTERS_HPP__

#include <expected>
#include <optional>

#include "cmspk/term/VtInput.hpp"

namespace cmspk::term {
// ================[ CODE BEGINS ]================
// Enums of errors
enum class VtInputFromCharactersError { CANNOT_ACCEPT_ANY_NEW_CHARACTER };

// Interface
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
    std::optional<VtInput> getData() { return data; }

    // maintenance
    void abort() {}
    void reset() { data = std::nullopt; }

  private:
    std::optional<VtInput> data = std::nullopt;
};
// ================[ END OF CODE ]================
}  // namespace cmspk::term
#endif
