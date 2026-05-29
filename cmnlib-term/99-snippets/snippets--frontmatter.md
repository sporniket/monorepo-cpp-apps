---
creation-date: 2026-05-15
copyright-years: 2026
copyright-authors: David SPORN
SPDX-License-Identifier: AGPL-3.0-or-later
---

# Frontmatters

A collection of frontmatter blocks for various formats.

_Naming scheme : `'frntmtr'+suffix`, e.g. `frntmtrmd`, `frntmtrhash`.

## frntmtrmd

Frontmatter for Markdown files, sporniket style.

```
---
creation-date: @@@{fn:today}
copyright-years: @@@{fn:thisyear}
copyright-authors: @@@{v:author:David SPORN}
SPDX-License-Identifier: @@@{v:licence:AGPL-3.0-or-later}
---
```

## frntmtrslshstar

Frontmatter for C/C++, Java, and such other languages using _«slash-star»_ (`/* ... */`) as multiple-line comments

```cpp
/* SPDX-License-Identifier: @@@{v:licence:AGPL-3.0-or-later} */
/****************************************

---
Copyright (C) @@@{fn:thisyear} @@@{v:author:David SPORN}
---
This is part of **@@@{v:projectname}**.
@@@{v:projectbrief}
****************************************/

```

## frntmtrdblslash

Frontmatter for C++, Java, and such other languages using _«double slash»_ (`//...`) as single-line comments


```cpp
// SPDX-License-Identifier: @@@{v:licence:AGPL-3.0-or-later}
// Copyright (C) @@@{fn:thisyear} @@@{v:author:David SPORN}
// ---
// This is part of **@@@{v:projectname}**.
// @@@{v:projectbrief}
// ---

```


## frntmtrhash

Frontmatter for file formats using _dash_ (`#...`) as single-line comments


```python
# SPDX-License-Identifier: @@@{v:licence:AGPL-3.0-or-later}
# Copyright (C) @@@{fn:thisyear} @@@{v:author:David SPORN}
# ---
# This is part of **@@@{v:projectname}**.
# @@@{v:projectbrief}
# ---

```

