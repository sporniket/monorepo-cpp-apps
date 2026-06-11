---
creation-date: 2026-06-11
copyright-years: 2026
copyright-authors: David SPORN
SPDX-License-Identifier: AGPL-3.0-or-later
---
# io, a library for writing I/O processing by sporniket

## Specifications

## How to build

_Assuming commands are run from the root directory of the project_

**Initialize/refresh cmake build directory**
```
cmake -B build --fresh .
```

**Build a specific target**
```
cmake --build build --target <target_name>
```

**Get the list of targets**
```
cmake --build build --target help
```

**Other tools**

* [How to run clang-format](./README--clang-format.md)
* [How to run doxygen](./README--doxygen.md)
