---
creation-date: 2026-05-25
copyright-years: 2026
copyright-authors: David SPORN
SPDX-License-Identifier: AGPL-3.0-or-later
---

# doxygen cheatsheet

## Cmake target

Assuming `build` is cmake build directory. The generated documentation will be located in `./build/doxygen/html`.

```
cmake --build build --target doc-generate
```

## Manual launch

The configuration file for this project is `doxygen-format.toml`, the general invocation is thus the following : 

```
doxygen doxyfile-format.toml
```

