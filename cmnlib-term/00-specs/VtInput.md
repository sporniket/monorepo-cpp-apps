---
creation-date: 2026-06-12
copyright-years: 2026
copyright-authors: David SPORN
SPDX-License-Identifier: AGPL-3.0-or-later
since: 0.0.2
---

# VtInput -- data model

> **The term "VT" is for "Virtual Terminal"**.

Specifies a representation of a virtual terminal input, that can be :

* either a _report_, usually after sending an output request to the Vt output, e.g. a report on the cursor position;
* or a _key stroke_, either encoded as a single char (1 to 27, 127), or as an escape sequence (`"\x1b[A"` for cursor up)
* or a printable character (either a single-octet printable character, or the first octet of a multi-octets character, depending of the character set encoding).
* or none of the formers, and as such will be ignored.

## Requirements

* [Specfication guidelines](../../README--specification-guidelines.md)

## Technical details

* VtInputKey
* VtInputReportType
* VtInputReport
* VtInputUnknown
* VtInput

### VtInputKey

Identifies a key stroke represented by a Vt sequence.

* By a "happy coïncidence", the numeric value of `CTRL + letter` and some special key (`RETURN`, `HTAB`, `ESCAPE`, and `BACKSPACE`) **will** be the value of their single octet representation in Vt.
  * `HTAB` and `RETURN` replace `CTRL_I` and `CTRL_M`, respectively
* By "design", any sequence of more than 1 octet mapped to a key **will** have a 32-bits value with the higher 16-bits value being the introducting character sequence (e.g. the CSI `escape+'['`), in other words, a CSI introduced key will have a value in the range `0x1b5b0000~0x1b5bffff`. **The actual values of the constants are subject to change at any time**

### VtInputReportType

Identifies a Vt report message type.

For now, only the report containing the cursor position is identified.

|Report type|example|
|---|---|
|A report containing the cursor position | "\x1b[24;80R"|

### VtInputReport

Model of a Vt report, like the cursor position.

|VtInputReport|
|---|
|+`VtInputReportType` type|
|+`std::vector<std::basic_string<char8_t>>` parameters|

### VtInputUnknown

An unprocessable input, like zero or any characters in the range 28 to 32.

A VtInputUnknown contains the actual value, and will usually be ignored.

### VtInput

A `std::variant` that can contain either a `char8_t`, or a `VtInputUnknown`, or a `VtInputReport` or a `VtInputKey`.

