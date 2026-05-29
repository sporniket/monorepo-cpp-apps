---
creation-date: 2026-05-28
copyright-years: 2026
copyright-authors: David SPORN
SPDX-License-Identifier: AGPL-3.0-or-later
---
# monorepo-cpp-apps

A monorepo for my cpp projects (small to mid-sized).

The integrality of this repository is published under the GNU Affero General Public Licence 3.0. A copy of the licence is available : [COPYING](COPYING)

## General organization

Cpp projects constist of _**main** code projects_ and _**library** code projects_

* **Library** code projects are stored in folders named like `cmnlib-xxxx`
* **Main** code projects are stored in folders named like `app-xxxx`

Each of those folders is a project, with their own CMake files, tool configurations and build folder.

A folder named `tool-config-ref` will store reference configurations of tools usually used by the projects (clang-format, doxygen, etc...). Depending on the tool, the refenrence files are to be simply copied to the target folder, or to be copied then edited.

## Programming rules

Read the file [README--programming-rules-cpp](README--programming-rules-cpp.md)

