---
creation-date: 2026-06-23
copyright-years: 2026
copyright-authors: David SPORN
SPDX-License-Identifier: AGPL-3.0-or-later
milestone: 0.0.2
---

# TtyInputSource should handle reports

Implement and pass the following tests from [the specifications](../00-specs/TtyInputSource.md) :

* TtyInputSource should return cursor position report

## Technical details

A TtyInputReport key is encoded as an escaped sequence using CSI (character 27 + `[`), semi-colon separated numeric values, and an operationad code encoded as a character.

This will require an advanced finite state machine framework, to be written beforehand.

