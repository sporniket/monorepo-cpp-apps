---
creation-date: 2026-06-12
copyright-years: 2026
copyright-authors: David SPORN
SPDX-License-Identifier: AGPL-3.0-or-later
since: 0.0.2
---

# TtyInputFromCharacters

## Assumptions and definitions

* **The term _byte_ is banned**, unless the design is intently targetting the smallest addressable unit of the processing unit ; most of the time, it is an octet, but it is mere coincidence ; e.g. read https://software-dl.ti.com/ccs/esd/documents/c2000_byte-accesses-with-the-c28x-cpu.html
* As a result of the previous statement, **this design is restricted to architectures where a byte is an octet**.
* A **character**, as read from low level stdin, is a 8-bits, unsigned value, a.k.a. an octet.
* We will use `char8_t` as the character type.
* **The term "TTY" is expected to designate a teminal inside a virtual terminal**, while it is used to speak of serial terminals.
* If possible, **supports 32-bits CPUs** 

## Require

* [TtyInput](./TtyInput.md)

## Overview

## Technical details

## Tests

### TtyInputFromCharacters should return a tty null on reading the zero octet

**given** TtyInputFromCharacters has been reset

**when** TtyInputFromCharacters is fed with the single character zero (`0x00`)

**then** TtyInputFromCharacters does not accept characters anymore

**then** the TtyInputFromCharacters will return a `std::variant` containing a `TtyInputNull`.

### TtyInputFromCharacters should return printable characters

**For each characters _Chr_ (`char8_t`) in range(32,256) excluding 127**

**given** TtyInputFromCharacters has been reset

**when** TtyInputFromCharacters is fed with the single character _Chr_

**then** TtyInputFromCharacters does not accept characters anymore

**then** the TtyInputFromCharacters will return a `std::variant` containing _Chr_.

### TtyInputFromCharacters should return keys on recognizing key sequences

**For each enum value _K_ in `TtyInputKey`**

**given** TtyInputFromCharacters has been reset

**when** TtyInputFromCharacters is fed with a character sequence that should be recognized as _K_

**then** TtyInputFromCharacters does not accept characters anymore

**then** the TtyInputFromCharacters will return a `std::variant` containing _K_.

### TtyInputFromCharacters should return tty report on recognizing cursor position report

**given** TtyInputFromCharacters has been reset

**when** TtyInputFromCharacters is fed with the character sequence "\x1b[24;80R"

**then** TtyInputFromCharacters does not accept characters anymore

**then** the TtyInputFromCharacters will return a `std::variant` containing the `TtyInputReport` of type `CURSOR_POSITION`, with 2 arguments "24" and "80".

### TtyInputFromCharacters should fall back to single-character conversion when a sequence is finally not recognized

**given** TtyInputFromCharacters has been reset

**when** TtyInputFromCharacters is fed with the character sequence "\x1bA"

**then** TtyInputFromCharacters does not accept characters anymore

**then** the TtyInputFromCharacters will containing the following sequence of data (`std::variant`) : 

* a `std::variant` containing the `TtyInputKey` value `ESCAPE` ;
* a `std::variant` containing the printable character `A` ;

### TtyInputFromCharacters should start a new sequence match when the current sequence is broken by the next character

**given** TtyInputFromCharacters has been reset

**when** TtyInputFromCharacters is fed with the character sequence "\x1b[\x1b[A"

**then** TtyInputFromCharacters does not accept characters anymore

**then** the TtyInputFromCharacters will containing the following sequence of data (`std::variant`) : 

* a `std::variant` containing the `TtyInputKey` value `ESCAPE` ;
* a `std::variant` containing the printable character `[` ;
* a `std::variant` containing the `TtyInputKey` value `ARROW_UP` ;

### TtyInputFromCharacters give access to available data while still accepting characters for the current sequence

**given** TtyInputFromCharacters has been reset

**when** TtyInputFromCharacters is fed with the character sequence "\x1b[\x1b"

**then** TtyInputFromCharacters still accept characters

**then** the TtyInputFromCharacters will containing the following sequence of data (`std::variant`) : 

* a `std::variant` containing the `TtyInputKey` value `ESCAPE` ;
* a `std::variant` containing the printable character `A` ;

### TtyInputFromCharacters should fall back to single-character conversion when it is aborted

**given** TtyInputFromCharacters has been reset and been fed with the character sequence "\x1b["

**when** TtyInputFromCharacters is aborted

**then** TtyInputFromCharacters does not accept characters anymore

**then** the TtyInputFromCharacters will containing the following sequence of data (`std::variant`) : 

* a `std::variant` containing the `TtyInputKey` value `ESCAPE` ;
* a `std::variant` containing the printable character `[` ;

### TtyInputFromCharacters clear its internal state when it is reset

**given** TtyInputFromCharacters has been reset and been fed with the character sequence "\x1b["

**when** TtyInputFromCharacters is reset

**then** TtyInputFromCharacters still accept characters

**then** the TtyInputFromCharacters contains no data

**when** TtyInputFromCharacters is fed with the single character `A`

**then** TtyInputFromCharacters does not accept characters anymore

**then** the TtyInputFromCharacters will return a `std::variant` containing `A`.


---
# sandbox

```cpp
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

while(converter.canAppend()) {
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

```

