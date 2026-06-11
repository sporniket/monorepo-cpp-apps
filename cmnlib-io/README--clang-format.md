---
creation-date: 2026-05-15
copyright-years: 2026
copyright-authors: David SPORN
SPDX-License-Identifier: AGPL-3.0-or-later
---

# clang-format cheatsheet

## Cmake target

Assuming `build` is cmake build directory.

**Dry run**
```
cmake --build build --target check-prettier
```

**Replace run**
```
cmake --build build --target prettier
```

## Manual launch

The formatting style configuration file for this project is `clang-format.yaml`, the general invocation is thus the following : 

**Dry run**

```
clang-format --verbose --style=file:clang-format.yaml <some_source_file>...
```


**Replace run**

Notice the `-i` switch.

```
clang-format --verbose --style=file:clang-format.yaml -i <some_source_file>...
```

