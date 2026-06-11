---
creation-date: 2026-06-02
copyright-years: 2026
copyright-authors: David SPORN
SPDX-License-Identifier: AGPL-3.0-or-later
---
# IoError

> DRAFT

## Overview

A model to describe an error triggered by an I/O operations.


```cpp
enum class IoErrorType {
  NOT_READY,END_OF_DATA,BAD ;
};

template<S> // require string (trait, concept ?)
struct BasicIoError{
  IoErrorType type;
  S message;
  std::vector<S> details;
};

using IoErrorAscii = BasicIoError<std::string>;
using IoError = BasicIoError<std::u32string>;
```

## Technical details

In a nutshell, an error is described with a _type_ (a.k.a. kind/class/family/...), a message, and some additional details.

By convention, the message will be a key to retrieve a localized message.

Also, the message and additional details could be represented as ASCII strings or Unicode strings.

## Additional types

* **IoErrorType**, an enum to describe the type/kind of error.

|name|description|
|---|---|
|`NOT_READY`|It is possible to retry later and get a success|
|`END_OF_DATA`|No more data to read/write|
|`BAD`|The source is definitely not operational|


## Not testable

This is an interface definition.

