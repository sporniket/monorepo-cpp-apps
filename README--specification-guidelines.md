---
creation-date: 2026-08-02
copyright-years: 2026
copyright-authors: David SPORN
SPDX-License-Identifier: AGPL-3.0-or-later
since: 0.0.2
---

# Specifications guidelines

> _This document is a **normative** reference that establish how specification **MUST** be written_
>
> Especially, it establishes :
>
> * the formally acceptable structure of a specification document
> * some common-sense definitions and conventions, to avoid repetition on each specification document.

* Formally acceptable structure of a specification document
  * General case : specification of a system by its behaviours
  * Limited case : data structures definitions
* Common-sense definitions and convention
  * Numeric range definition
  * Unit of data storage and memory model


## Formally acceptable structure of a specification document

### General case : specification of a system by its behaviours

The full structure of a specification document reflects the order of importance given to various aspects of the document. In other words, the various parts of the documents are listed in decreasing order of importance.

```
Title
  (Abstract, OPTIONAL untitled section)
  Requirements (MANDATORY, links to other specs)
  Behaviours (MANDATORY, specifications of all the observable behaviours using _Gherkin_ notation, optionally grouped by user/job stories)
  Technical details (OPTIONAL, What cannot be describeld by the behaviours)
```

**Nota Bene**

* The _Requirements_ part **WILL always** references this guideline.

### Limited case : data structures definitions

A system **MAY** expose complex and/or various data structures without associated behaviours, typically for data exchange. In this case, the structure of the specification document is a little bit different.

```
Title -- Data model
  (Abstract, MANDATORY untitled section, listing the concepts needing a data structure)
  Requirements (MANDATORY, links to other specs)
  Technical details (MANDATORY, a table of content listing each data structure being specified, usually in a logical order : put independant structures first, then other structures depending on the formers)
```

**Nota Bene**

* The _Requirements_ part **WILL always** references this guideline.

## Common-sense definitions and convention

### Numeric range definition

When specifying a numeric range between two bounds, the lower bound _A_ and the upper bound _B_, it will be understood that _A_ is included and _B_ is excluded.

E.g. _"The value of X is in range 32 to 128"_ means `32 <= X and X < 128`

It is not restricted to integer values.

E.g. _"The value of X is in range 1.414 to 3.14"_ means `1.414 <= X and X < 3.14` (note that comparing floating point values should be done properly)

### Unit of data storage and memory model

#### NO BYTE, unless explicitely targeted

* **The term _byte_ is banned**, unless the design is intently **and explicitely** targetting the smallest addressable unit of the processing unit ; most of the time, it is an octet, but it is mere coincidence ; e.g. read https://software-dl.ti.com/ccs/esd/documents/c2000_byte-accesses-with-the-c28x-cpu.html
* As a result of the previous statement, **all the specifications are restricted to architectures where a byte is an octet**.

**To Be Checked** : prevent the compiler to proceed when the target architecture does not use 8-bits bytes.

```cpp
// Requires C++11 at least.
#include <climits>

static_assert(CHAR_BIT == 8, "This program requires that a byte MUST be an octet");
```

#### Octets and characters

* An **octet** is a unit of storage (a bundle of 8 bits). The representation of an actual stored value is an unsigned number in the range 0 (included) to 256 (excluded).
* A **character**, as read from low level stdin, **is an octet**.
* We will use the standard `char8_t` as the character type.
* At some point, I will define `octet` as an alias of `char8_t`.

#### 32-bits support

* If possible, **all the specifications will supports 32-bits CPUs** meaning using at most 32-bits values, unless there is a compelling reason to do otherwise (e.g. using a standard data structure using 64-bits values).

