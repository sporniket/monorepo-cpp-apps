---
creation-date: 2026-06-23
copyright-years: 2026
copyright-authors: David SPORN
SPDX-License-Identifier: AGPL-3.0-or-later
milestone: 0.0.2
---

# TtyInputSource should handle io errors


Implement and pass the following tests from [the specifications](../00-specs/TtyInputSource.md) :

* TtyInputSource should reuse unconsumed reads from the character data source

# Technical details

The buffer used to memorize unconsumed data will store the raw `std::expected<char8_t,BasicIoError<std::u32string>>`.
