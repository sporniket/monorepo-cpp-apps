---
creation-date: 2026-06-23
copyright-years: 2026
copyright-authors: David SPORN
SPDX-License-Identifier: AGPL-3.0-or-later
---

# TtyInputSource should handle keys

Implement and pass the following tests from [the specifications](../00-specs/TtyInputSource.md) :

* TtyInputSource should return keys

## Technical details

A key is encoded either as a single character value, or as an escaped sequence using CSI (character 27 + `[`).

Use a way that will be easy to expend, namely use a finite state machine build from the various sequence to recognize : it will
make supporting new keys easier (registering a new sequence) than directly coding ad-hoc support with switch/case-ing.

This will require a finite state machine framework, to be written beforehand.

