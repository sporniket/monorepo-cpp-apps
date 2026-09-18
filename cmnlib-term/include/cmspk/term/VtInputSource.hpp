// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 David SPORN
// ---
// This is part of **term library by sporniket**.
// A collection of utilities for writing terminal-hosted applications.
// ---

#ifndef __CMSPK__TERM__VTINPUTSOURCE_HPP__
#define __CMSPK__TERM__VTINPUTSOURCE_HPP__

// standard libs
#include <cstdint>
#include <expected>
#include <memory>
#include <optional>

// project
#include "cmspk/io/DataSource.hpp"
#include "cmspk/term/VtInputFromCharacters.hpp"

namespace cmspk::term {
// ================[ CODE BEGINS ]================

/************************************************
As the name implies, a data source of `cmspk::term::VtInput` <i>(NOT Copiable)</i>.

Typical application :

```cpp
std::unique_ptr<cmspk::io::BasicDataSource<char8_t, char8_t>> usuallyStandardInput ; // TO BE GIVEN
cmspk::term::VtInputSource myInput(usuallyStandardInput) ;

// ... main loop
bool running = true;
while(running) {
  std::expected<cmspk::term::VtInput,cmspk::io::BasicIoError<char8_t>> nextInput = myInput.next() ;
  if (!nextInput) { continue ; }

  //... process the actual value of myInput
  if (std::holds_alternative<cmspk::term::VtInputUnknown>(*nextInput)) { continue; }
  // etc...
}

```

Under the hood, it reads from a provided data source of raw characters `char8_t` and manages a `cmspk::term::VtInputFromCharacters`.


## Requirements

* * [Specfication guidelines](../../README--specification-guidelines.md)
* * cmspk::term::VtInput
* * cmspk::term::VtInputFromCharacters
* * cmspk::io::DataSource


## Behaviours

> _BEGIN extracted from `TestRunner--VtInputSource.cpp`_

<hr>

### VtInputSource, should_process_single_octet_values__unknown_values ###

> This is the fall-back behaviour. The list [28,29,30,31] is what remains unknown when all other behaviours have been implemented.

__given__ a character data source that will return the sequence : `[28,29,30,31]`.

__given__ the VtInputSource under test is plugged to that character data source.

__when__ reading the next VtInput with `next()` as many time as the length of the sequence.

__then__ each time the VtInputSource will return a `std::variant` containing a `VtInputUnknown` having the expected `rawValue`.

__then__ reading the next VtInput returns and end of file error.

<hr>

### VtInputSource, should_process_single_octet_values__printable_characters_and_keys ###

__given__ a character data source that will return a mixed list of single-octet printable characters and single-octet keys (except ESCAPE).

__given__ the VtInputSource under test is plugged to that character data source.

__when__ reading the next VtInput with `next()` as many time as the length of the sequence.

__then__ each time the VtInputSource will return a `std::variant` containing the expected `char8_t` or `VtInputKey`.

__then__ reading the next VtInput returns and end of file error.

<hr>

> _END extracted from `TestRunner--VtInputSource.cpp`_

************************************************/
class VtInputSource : public cmspk::io::BasicDataSource<cmspk::term::VtInput, char8_t> {
  public:
    VtInputSource(std::unique_ptr<cmspk::io::BasicDataSource<char8_t, char8_t>> source,
                  std::unique_ptr<cmspk::term::VtInputFromCharacters> converter =
                      std::unique_ptr<cmspk::term::VtInputFromCharacters>(new cmspk::term::VtInputFromCharacters()))
        : source(std::move(source)), converter(std::move(converter)) {}

    virtual std::expected<cmspk::term::VtInput, cmspk::io::BasicIoError<char8_t>> next() {
        if (converter->canGetData()) {
            return converter->getData().value();
        }
        if (!converter->canAppend()) {
            converter->reset();
        }
        std::optional<cmspk::io::BasicIoError<char8_t>> error = std::nullopt;
        while (!converter->canGetData()) {
            std::expected<char8_t, cmspk::io::BasicIoError<char8_t>> nextChar = source->next();
            if (!nextChar) {
                error = nextChar.error();
                converter->abort();
                break;
            }
            converter->append(*nextChar);
        }
        if (converter->canGetData()) {
            return converter->getData().value();
        }
        if (error) {
            return std::unexpected(*error);
        }
        // should not happen
        return std::unexpected(cmspk::io::BasicIoError<char8_t>{.type = cmspk::io::IoErrorType::NOT_READY, .message = u8"not.ready", .details = {}});
    }
    virtual bool hasNext() { return converter->canGetData() || converter->canAppend() || source->hasNext(); }

    virtual ~VtInputSource() {}
    /**
     * Copy operation (rule of 5) : NOT copiable.
     */
    VtInputSource(const VtInputSource&) = delete;

    /**
     * Copy operator (rule of 5) : NOT copiable.
     */
    VtInputSource& operator=(const VtInputSource&) = delete;

    /**
     * Move operation (rule of 5).
     */
    VtInputSource(VtInputSource&&) = default;

    /**
     * Move operator (rule of 5).
     */
    VtInputSource& operator=(VtInputSource&&) = default;

  private:
    std::unique_ptr<cmspk::io::BasicDataSource<char8_t, char8_t>> source;
    std::unique_ptr<cmspk::term::VtInputFromCharacters> converter;
};

// ================[ END OF CODE ]================
}  // namespace cmspk::term
#endif
