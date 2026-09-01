// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 David SPORN
// ---
// This is part of **term library by sporniket**.
// A collection of utilities for writing terminal-hosted applications.
// ---

#ifndef __CMSPK__TERM__VTINPUT_HPP__
#define __CMSPK__TERM__VTINPUT_HPP__

#include <variant>

namespace cmspk::term {
// ================[ CODE BEGINS ]================

struct VtInputUnknown {
    char8_t rawValue;
};

using VtInput = std::variant<VtInputUnknown>;

// ================[ END OF CODE ]================
}  // namespace cmspk::term
#endif
