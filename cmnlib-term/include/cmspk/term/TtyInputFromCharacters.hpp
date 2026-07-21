// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 David SPORN
// ---
// This is part of **term library by sporniket**.
// A collection of utilities for writing terminal-hosted applications.
// ---

#ifndef __CMSPK__TERM__TTYINPUTFROMCHARACTERS_HPP__
#define __CMSPK__TERM__TTYINPUTFROMCHARACTERS_HPP__

#include <string>

#include "cmspk/io/DataSource.hpp"

namespace cmspk::term {
// ================[ CODE BEGINS ]================

/* draft */
class AsciiTtyInputFromRawCharacters {
  public:
    bool canAppend() ; // true when more data is needed to obtain a ttyInput, false when data is available.
    void append(char8_t) ; //feeds the converter
    void abort() ; // any appended characters will be made available
    bool hasData() ; //true as long as all the data has not be consumed (when finally there was nothing, all the chars can be retrieved)
    std::optional<AsciiTtyInput> getData() ; //when there are non consumed data available, returns a non empty value
    void reset() ; //when all data has been retrieved
};
/* application */
/* BEGIN */
//given
DataSource<char8_t> rawCharacters;
AsciiTtyInputFromRawCharacters converter;
std::optionnal<IoError<?>> ioError = empty();

while(converter.canAppend) {
  auto nextChar = rawCharacters.next() ;
  if(!nextChar) {
    ioError = nextChar.error_value() ;
    converter.abort();
    break;
  }
  converter.append(nextChar) ;
}
while(converter.hasData()) {
  return converter.getData() ; // imagine the host call in a loop.
}
if(ioError is not empty) {
  return ioError.value()
}
converter.reset();
/* END */

// ================[ END OF CODE ]================
}  // namespace cmspk::term
#endif

