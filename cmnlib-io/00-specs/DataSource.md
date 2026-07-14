---
creation-date: 2026-06-02
copyright-years: 2026
copyright-authors: David SPORN
SPDX-License-Identifier: AGPL-3.0-or-later
since: 0.0.1
---
# DataSource

## History

* **0.0.2** : replace String class by character class in template arg.

## Overview

An interface to model a source of arbitrary type.

## Requirements

* [IoError](IoError.md)

## Technical details

```
// Require IoError
// T type of data returned
// CharT type of character (should be either char8_t or char32_t)
template<class T, class CharT>
class BasicDataSource{
  public:
    virtual std::expected<T,BasicIoError<CharT>> next() = 0;
    virtual bool hasNext() {return true;} // override to avoid a read expected to fail
    
    virtual ~DataSource() {}
    // rule of 5 use =default
};

// The actual nominal type will use BasicIoError<char32_t>.
template<class T>
using DataSource = BasicDataSource<T,char32_t>;
```

## Additional types

> None

## Not testable

This is an interface definition.

