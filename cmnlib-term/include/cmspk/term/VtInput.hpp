// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 David SPORN
// ---
// This is part of **term library by sporniket**.
// A collection of utilities for writing terminal-hosted applications.
// ---

#ifndef __CMSPK__TERM__VTINPUT_HPP__
#define __CMSPK__TERM__VTINPUT_HPP__

// standard libs
#include <variant>

namespace cmspk::term {
// ================[ CODE BEGINS ]================
/************************************************
An unprocessable input, like zero or any characters in the range 28 to 32.

A VtInputUnknown contains the actual value, and will usually be ignored.
************************************************/
struct VtInputUnknown {
    char8_t rawValue;
};

/************************************************
A representation of a virtual terminal input

It can be :
<ul>
<li>**TODO** either a _report_, usually after sending an output request to the Vt output, e.g. a report on the cursor position ;</li>
<li>**TODO** or a _key stroke_, either encoded as a single char (1 to 27, 127), or as an escape sequence (`"\x1b[A"` for cursor up) ;</li>
<li>**TODO** or a _printable character_ (either a single-octet printable character, or the first octet of a multi-octets character, depending of the character
set encoding) ;</li> <li>or none of the formers, i.e. an _unknown_ input, and as such will be ignored ;</li>
</ul>
************************************************/
using VtInput = std::variant<VtInputUnknown>;

// ================[ END OF CODE ]================
}  // namespace cmspk::term
#endif
