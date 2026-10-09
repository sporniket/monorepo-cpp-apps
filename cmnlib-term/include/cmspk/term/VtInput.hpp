// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 David SPORN
// ---
// This is part of **term library by sporniket**.
// A collection of utilities for writing terminal-hosted applications.
// ---

#ifndef __CMSPK__TERM__VTINPUT_HPP__
#define __CMSPK__TERM__VTINPUT_HPP__

// standard libs
#include <cstdint>
#include <map>
#include <string>
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
    ctrl_space = 0,
    ctrl_a,
    ctrl_b,
    ctrl_c,
    ctrl_d,
    ctrl_e,
    ctrl_f,
    ctrl_g,
    ctrl_h,
    htab,
    ctrl_j,
    ctrl_k,
    ctrl_l,
    return_key,  // because return is a C++ keyword
    ctrl_n,
    ctrl_o,
    ctrl_p,
    ctrl_q,
    ctrl_r,
    ctrl_s,
    ctrl_t,
    ctrl_u,
    ctrl_v,
    ctrl_w,
    ctrl_x,
    ctrl_y,
    ctrl_z,
    escape,
    backspace = 127,
    // and now keys mapped to multi-octets sequence (with CSI)
    // -- arrows
    arrow_up = 0x1b5b0000,
    arrow_down,
    arrow_left,
    arrow_right
};

/************************************************
Model of a Vt cursor position report.
************************************************/
struct VtInputCursorPositionReport {
    /**
     * Row, i.e. y position (0-based or 1-based ?).
     */
    uint16_t row;
    /**
     * Column, i.e. x position (0-based or 1-based ?).
     */
    uint16_t col;
};
/************************************************
Model of a Vt report, like the cursor position.
************************************************/
using VtInputReport = std::variant<VtInputCursorPositionReport>;

/************************************************
A representation of a virtual terminal input

It can be :
<ul>
<li>**TODO** either a _report_, usually after sending an output request to the Vt output, e.g. a report on the cursor position ;</li>
<li>or a _key stroke_, either encoded as a single char (1 to 27, 127), or as an escape sequence (`"\x1b[A"` for cursor up) ;</li>
<li>or a _printable character_ (either a single-octet printable character, or the first octet of a multi-octets character, depending of the character
set encoding) ;</li> <li>or none of the formers, i.e. an _unknown_ input, and as such will be ignored ;</li>
</ul>
************************************************/
using VtInput = std::variant<VtInputReport, VtInputKey, char8_t, VtInputUnknown>;

/************************************************
Registry of known multi-char sequences mapped to a VtInputKey.
************************************************/
const std::map<std::basic_string<char8_t>, VtInputKey> known_multi_char_key_sequences{
    {u8"\x1b[A", VtInputKey::arrow_up},
    {u8"\x1b[B", VtInputKey::arrow_down},
    {u8"\x1b[C", VtInputKey::arrow_left},
    {u8"\x1b[D", VtInputKey::arrow_right},
};

/************************************************
Registry of VtInputKey names, us-ascii version, should be useless in C++26 with reflection.
************************************************/
static const std::map<VtInputKey, std::basic_string<char8_t>> vt_input_key_names_ascii{
    {VtInputKey::ctrl_space, u8"ctrl_space"},  {VtInputKey::ctrl_a, u8"ctrl_a"},
    {VtInputKey::ctrl_b, u8"ctrl_b"},          {VtInputKey::ctrl_c, u8"ctrl_c"},
    {VtInputKey::ctrl_d, u8"ctrl_d"},          {VtInputKey::ctrl_e, u8"ctrl_e"},
    {VtInputKey::ctrl_f, u8"ctrl_f"},          {VtInputKey::ctrl_g, u8"ctrl_g"},
    {VtInputKey::ctrl_h, u8"ctrl_h"},          {VtInputKey::htab, u8"htab"},
    {VtInputKey::ctrl_j, u8"ctrl_*"},          {VtInputKey::ctrl_k, u8"ctrl_k"},
    {VtInputKey::ctrl_l, u8"ctrl_l"},          {VtInputKey::return_key, u8"return_key"},
    {VtInputKey::ctrl_n, u8"ctrl_n"},          {VtInputKey::ctrl_o, u8"ctrl_o"},
    {VtInputKey::ctrl_p, u8"ctrl_p"},          {VtInputKey::ctrl_q, u8"ctrl_q"},
    {VtInputKey::ctrl_r, u8"ctrl_r"},          {VtInputKey::ctrl_s, u8"ctrl_s"},
    {VtInputKey::ctrl_t, u8"ctrl_t"},          {VtInputKey::ctrl_u, u8"ctrl_u"},
    {VtInputKey::ctrl_v, u8"ctrl_v"},          {VtInputKey::ctrl_w, u8"ctrl_w"},
    {VtInputKey::ctrl_x, u8"ctrl_x"},          {VtInputKey::ctrl_y, u8"ctrl_y"},
    {VtInputKey::ctrl_z, u8"ctrl_z"},          {VtInputKey::escape, u8"escape"},
    {VtInputKey::backspace, u8"backspace"},    {VtInputKey::arrow_up, u8"arrow_up"},
    {VtInputKey::arrow_down, u8"arrow_down"},  {VtInputKey::arrow_left, u8"arrow_left"},
    {VtInputKey::arrow_right, u8"arrow_right"}};

/************************************************
Registry of VtInputKey names, should be useless in C++26 with reflection.
************************************************/
static const std::map<VtInputKey, std::basic_string<char32_t>> vt_input_key_names{
    {VtInputKey::ctrl_space, U"ctrl_space"},  {VtInputKey::ctrl_a, U"ctrl_a"},
    {VtInputKey::ctrl_b, U"ctrl_b"},          {VtInputKey::ctrl_c, U"ctrl_c"},
    {VtInputKey::ctrl_d, U"ctrl_d"},          {VtInputKey::ctrl_e, U"ctrl_e"},
    {VtInputKey::ctrl_f, U"ctrl_f"},          {VtInputKey::ctrl_g, U"ctrl_g"},
    {VtInputKey::ctrl_h, U"ctrl_h"},          {VtInputKey::htab, U"htab"},
    {VtInputKey::ctrl_j, U"ctrl_*"},          {VtInputKey::ctrl_k, U"ctrl_k"},
    {VtInputKey::ctrl_l, U"ctrl_l"},          {VtInputKey::return_key, U"return_key"},
    {VtInputKey::ctrl_n, U"ctrl_n"},          {VtInputKey::ctrl_o, U"ctrl_o"},
    {VtInputKey::ctrl_p, U"ctrl_p"},          {VtInputKey::ctrl_q, U"ctrl_q"},
    {VtInputKey::ctrl_r, U"ctrl_r"},          {VtInputKey::ctrl_s, U"ctrl_s"},
    {VtInputKey::ctrl_t, U"ctrl_t"},          {VtInputKey::ctrl_u, U"ctrl_u"},
    {VtInputKey::ctrl_v, U"ctrl_v"},          {VtInputKey::ctrl_w, U"ctrl_w"},
    {VtInputKey::ctrl_x, U"ctrl_x"},          {VtInputKey::ctrl_y, U"ctrl_y"},
    {VtInputKey::ctrl_z, U"ctrl_z"},          {VtInputKey::escape, U"escape"},
    {VtInputKey::backspace, U"backspace"},    {VtInputKey::arrow_up, U"arrow_up"},
    {VtInputKey::arrow_down, U"arrow_down"},  {VtInputKey::arrow_left, U"arrow_left"},
    {VtInputKey::arrow_right, U"arrow_right"}};

// ================[ END OF CODE ]================
}  // namespace cmspk::term
#endif
