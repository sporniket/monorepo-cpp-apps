// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 David SPORN
// ---
// This is part of **term library by sporniket**.
// A collection of utilities for writing terminal-hosted applications.
// ---

#ifndef __CMSPK__TERM__VTINPUTSOURCE_HPP__
#define __CMSPK__TERM__VTINPUTSOURCE_HPP__

#include <string>

#include "cmspk/io/DataSource.hpp"

#include "cmspk/term/VtInput.hpp"
#include "cmspk/term/VtInputFromCharacters.hpp"

namespace cmspk::term {
// ================[ CODE BEGINS ]================

// TODO AsciiStdinDataSource = DataSource<char8_t> encapsulating read(STDIN,...)
// TODO BasicVtInputSource<CharT> --> AsciiVtInputSource = BasicVtInputSource<char8_t>

template<class CharT, class S>
class BasicVtInputSource : public DataSource<BasicVtInput<CharT>> {
  public:
    virtual std::expected<BasicVtInput<CharT>, BasicIoError<S>> next() ;
    BasicVtInputSource(BasicDataSource<CharT,S> source) : source(source) {};
    virtual ~BasicVtInputSource() {} ;

    /**
     * Copy operation (rule of 5).
     */
    BasicVtInputSource(const BasicVtInputSource&) = default;

    /**
     * Copy operator (rule of 5).
     */
    BasicVtInputSource& operator=(const BasicVtInputSource&) = default;

    /**
     * Move operation (rule of 5).
     */
    BasicVtInputSource(BasicVtInputSource&&) = default;

    /**
     * Move operator (rule of 5).
     */
    BasicVtInputSource& operator=(BasicVtInputSource&&) = default;


  private:
    BasicDataSource<CharT,S> source ;
};

template<class S>
using AsciiVtInputSource = BasicVtInputSource<char8_t, S>;

template<>
class AsciiVtInputSource<std::u32string> ;

// TODO VtInput.cpp -> 
/*
std::expected<BasicVtInput<char8_t>, BasicIoError<std::u32string>> AsciiVtInputSource<std::u32string>::next() {
  std::expected<char8_t, BasicIoError<std::u32string>> nextOctet = source.next() ;
  if (nextOctet) {
    doSomethingWith(nextOctet.value())
  } else {
    return nextOctet.error();
  }
} 
*/

// ================[ END OF CODE ]================
}  // namespace cmspk::term
#endif

