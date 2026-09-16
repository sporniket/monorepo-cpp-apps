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
Identifies a key stroke represented by a Vt sequence.

* By a "happy coïncidence", the numeric value of `CTRL + letter` and some special key (`RETURN`, `HTAB`, `ESCAPE`, and `BACKSPACE`) **will** be the value of
their single octet representation in Vt.
  * `HTAB` and `RETURN` replace `CTRL_I` and `CTRL_M`, respectively
  * `CTRL_SPACE` has numeric value zero.
* By "design", any sequence of more than 1 octet mapped to a key **will** have a 32-bits value with the higher 16-bits value being the introducting character
sequence (e.g. the CSI `escape+'['`), in other words, a CSI introduced key will have a value in the range `0x1b5b0000~0x1b5bffff`. **The actual values of the
constants are subject to change at any time**
************************************************/
enum class VtInputKey {
    CTRL_SPACE = 0,
    CTRL_A,
    CTRL_B,
    CTRL_C,
    CTRL_D,
    CTRL_E,
    CTRL_F,
    CTRL_G,
    CTRL_H,
    HTAB,
    CTRL_J,
    CTRL_K,
    CTRL_L,
    RETURN,
    CTRL_N,
    CTRL_O,
    CTRL_P,
    CTRL_Q,
    CTRL_R,
    CTRL_S,
    CTRL_T,
    CTRL_U,
    CTRL_V,
    CTRL_W,
    CTRL_X,
    CTRL_Y,
    CTRL_Z,
    ESCAPE,
    BACKSPACE = 127
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
using VtInput = std::variant<VtInputKey, char8_t, VtInputUnknown>;

// ================[ END OF CODE ]================
}  // namespace cmspk::term
#endif
