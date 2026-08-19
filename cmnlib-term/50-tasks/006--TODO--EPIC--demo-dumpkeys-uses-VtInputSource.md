---
creation-date: 2026-08-19
copyright-years: 2026
copyright-authors: David SPORN
SPDX-License-Identifier: AGPL-3.0-or-later
milestone: 0.0.2
---

# demo-dumpkeys uses VtInputSource

# Requirement

* [EPIC 001](./001--TODO--tty-input-source.md)

# Behaviours

* On each input value, the program shows the nature of the input (none, printable character,
  keys, report), and the value (usually numeric, except for printable characters in the
  range 32~256, and for keys, the name of the key combination)
* On typing "CTRL+Q", the program ends.
