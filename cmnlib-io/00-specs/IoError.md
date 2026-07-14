---
creation-date: 2026-06-02
copyright-years: 2026
copyright-authors: David SPORN
SPDX-License-Identifier: AGPL-3.0-or-later
since: 0.0.1
---
# IoError

## History

* **0.0.2** : replace String class by character class in template arg.

## Overview

A model to describe an error triggered by an I/O operations.


```cpp
enum class IoErrorType {
  NOT_READY,END_OF_DATA,BAD ;
};

template<class CharT> // usually char8_t or char32_t
struct BasicIoError{
  IoErrorType type;
  std::basic_string<CharT> message;
  std::vector<std::basic_string<CharT>> details;
};

using IoErrorAscii = BasicIoError<char8_t>;
using IoError = BasicIoError<char32_t>;
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

