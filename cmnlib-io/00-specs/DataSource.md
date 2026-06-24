---
creation-date: 2026-06-02
copyright-years: 2026
copyright-authors: David SPORN
SPDX-License-Identifier: AGPL-3.0-or-later
since: 0.0.1
---
# DataSource

> DRAFT

## Overview

An interface to model a source of arbitrary type.

## Requirements

* [IoError](IoError.md)

## Technical details

```
// Require IoError
// T type of data returned
// S type of string (should be either std::string or std::u32string)
template<class T, class S>
class BasicDataSource{
  public:
    virtual std::expected<T,BasicIoError<S>> next() = 0;
    virtual bool hasNext() {return true;} // override to avoid a read expected to fail
    
    virtual ~DataSource() {}
    // rule of 5 use =default
};

// The actual nominal type will use BasicIoError<std::u32string>.
template<class T>
using DataSource = BasicDataSource<T,std::u32string>;
```

## Additional types

> None

## Not testable

This is an interface definition.

