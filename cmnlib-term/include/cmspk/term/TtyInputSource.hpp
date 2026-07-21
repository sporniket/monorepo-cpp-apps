// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 David SPORN
// ---
// This is part of **term library by sporniket**.
// A collection of utilities for writing terminal-hosted applications.
// ---

#ifndef __CMSPK__TERM__TTYINPUTSOURCE_HPP__
#define __CMSPK__TERM__TTYINPUTSOURCE_HPP__

#include <string>

#include "cmspk/io/DataSource.hpp"

#include "cmspk/term/TtyInput.hpp"
#include "cmspk/term/TtyInputFromCharacters.hpp"

namespace cmspk::term {
// ================[ CODE BEGINS ]================

// TODO AsciiStdinDataSource = DataSource<char8_t> encapsulating read(STDIN,...)
// TODO BasicTtyInputSource<CharT> --> AsciiTtyInputSource = BasicTtyInputSource<char8_t>

template<class CharT, class S>
class BasicTtyInputSource : public DataSource<BasicTtyInput<CharT>> {
  public:
    virtual std::expected<BasicTtyInput<CharT>, BasicIoError<S>> next() ;
    BasicTtyInputSource(BasicDataSource<CharT,S> source) : source(source) {};
    virtual ~BasicTtyInputSource() {} ;

    /**
     * Copy operation (rule of 5).
     */
    BasicTtyInputSource(const BasicTtyInputSource&) = default;

    /**
     * Copy operator (rule of 5).
     */
    BasicTtyInputSource& operator=(const BasicTtyInputSource&) = default;

    /**
     * Move operation (rule of 5).
     */
    BasicTtyInputSource(BasicTtyInputSource&&) = default;

    /**
     * Move operator (rule of 5).
     */
    BasicTtyInputSource& operator=(BasicTtyInputSource&&) = default;


  private:
    BasicDataSource<CharT,S> source ;
};

template<class S>
using AsciiTtyInputSource = BasicTtyInputSource<char8_t, S>;

template<>
class AsciiTtyInputSource<std::u32string> ;

// TODO TtyInput.cpp -> 
/*
std::expected<BasicTtyInput<char8_t>, BasicIoError<std::u32string>> AsciiTtyInputSource<std::u32string>::next() {
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

