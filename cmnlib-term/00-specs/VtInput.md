---
creation-date: 2026-06-12
copyright-years: 2026
copyright-authors: David SPORN
SPDX-License-Identifier: AGPL-3.0-or-later
since: 0.0.2
---

# VtInput

## Assumptions and definitions

* **The term "VT" is "Virtual Verminal"**.
* **The term _byte_ is banned**, unless the design is intently targetting the smallest addressable unit of the processing unit ; most of the time, it is an octet, but it is mere coincidence ; e.g. read https://software-dl.ti.com/ccs/esd/documents/c2000_byte-accesses-with-the-c28x-cpu.html
* As a result of the previous statement, **this design is restricted to architectures where a byte is an octet**.
* A **character**, as read from low level stdin, is a 8-bits, unsigned value, a.k.a. an octet.
* We will use `char8_t` as the character type.
* If possible, **supports 32-bits CPUs** 

## Overview

Specifies a representation of a virtual terminal input, that can be :

* either a _report_, usually after sending an output request to the Vt output, e.g. a report on the cursor position;
* or a _key stroke_, either encoded as a single char (1 to 27, 127), or as an escape sequence (`"\x1b[A"` for cursor up)
* or the null byte (zero), that will be ignored
* or a printable character (either a single-octet printable character, or the first octet of a multi-octets character, depending of the character set encoding).

## Technical details

* VtInputKey
* VtInputReporVtpe
* VtInputReport
* VtInputNull

### VtInputKey

Identifies a key stroke represented by a Vt sequence.

* By a "happy coïncidence", the numeric value of `CTRL + letter` and some special key (`RETURN`, `HTAB`, `ESCAPE`, and `BACKSPACE`) **will** be the value of their single octet representation in Vt.
  * `HTAB` and `RETURN` replace `CTRL_I` and `CTRL_M`, respectively
* By "design", any sequence of more than 1 octet mapped to a key **will** have a 32-bits value with the higher 16-bits value being the introducting character sequence (e.g. the CSI `escape+'['`), in other words, a CSI introduced key will have a value in the range `0x1b5b0000~0x1b5bffff`. **The actual values of the constants are subject to change at any time**

### VtInputReporVtpe

Identifies a Vt report message type.

For now, only the report containing the cursor position is identified.

|Report type|example|
|---|---|
|A report containing the cursor position | "\x1b[24;80R"|

### VtInputReport

Model of a Vt report, like the cursor position.

|VtInputReport|
|---|
|+`VtInputReporVtpe` type|
|+`std::vector<std::basic_string<char8_t>>` parameters|

### VtInputNull

An empty type, to be returned when the datasource get zero (0), which should not happen in a terminal input.

## Tests

_None, this is a module of models_

