---
creation-date: 2026-06-12
copyright-years: 2026
copyright-authors: David SPORN
SPDX-License-Identifier: AGPL-3.0-or-later
since: 0.0.2
---

# VtInputFromCharacters

## Assumptions and definitions

* **The term "VT" is "Virtual Verminal"**.
* **The term _byte_ is banned**, unless the design is intently targetting the smallest addressable unit of the processing unit ; most of the time, it is an octet, but it is mere coincidence ; e.g. read https://software-dl.ti.com/ccs/esd/documents/c2000_byte-accesses-with-the-c28x-cpu.html
* As a result of the previous statement, **this design is restricted to architectures where a byte is an octet**.
* A **character**, as read from low level stdin, is a 8-bits, unsigned value, a.k.a. an octet.
* We will use `char8_t` as the character type.
* If possible, **supports 32-bits CPUs** 

## Require

* [VtInput](./VtInput.md)

## Overview

## Technical details

## Tests

### VtInputFromCharacters should return a Vt null on reading the zero octet

**given** VtInputFromCharacters has been reset

**when** VtInputFromCharacters is fed with the single character zero (`0x00`)

**then** VtInputFromCharacters does not accept characters anymore

**then** the VtInputFromCharacters will return a `std::variant` containing a `VtInputNull`.

### VtInputFromCharacters should return printable characters

**For each characters _Chr_ (`char8_t`) in range(32,256) excluding 127**

**given** VtInputFromCharacters has been reset

**when** VtInputFromCharacters is fed with the single character _Chr_

**then** VtInputFromCharacters does not accept characters anymore

**then** the VtInputFromCharacters will return a `std::variant` containing _Chr_.

### VtInputFromCharacters should return keys on recognizing key sequences

**For each enum value _K_ in `VtInputKey`**

**given** VtInputFromCharacters has been reset

**when** VtInputFromCharacters is fed with a character sequence that should be recognized as _K_

**then** VtInputFromCharacters does not accept characters anymore

**then** the VtInputFromCharacters will return a `std::variant` containing _K_.

### VtInputFromCharacters should return Vt report on recognizing cursor position report

**given** VtInputFromCharacters has been reset

**when** VtInputFromCharacters is fed with the character sequence "\x1b[24;80R"

**then** VtInputFromCharacters does not accept characters anymore

**then** the VtInputFromCharacters will return a `std::variant` containing the `VtInputReport` of type `CURSOR_POSITION`, with 2 arguments "24" and "80".

### VtInputFromCharacters should fall back to single-character conversion when a sequence is finally not recognized

**given** VtInputFromCharacters has been reset

**when** VtInputFromCharacters is fed with the character sequence "\x1bA"

**then** VtInputFromCharacters does not accept characters anymore

**then** the VtInputFromCharacters will containing the following sequence of data (`std::variant`) : 

* a `std::variant` containing the `VtInputKey` value `ESCAPE` ;
* a `std::variant` containing the printable character `A` ;

### VtInputFromCharacters should start a new sequence match when the current sequence is broken by the next character

**given** VtInputFromCharacters has been reset

**when** VtInputFromCharacters is fed with the character sequence "\x1b[\x1b[A"

**then** VtInputFromCharacters does not accept characters anymore

**then** the VtInputFromCharacters will containing the following sequence of data (`std::variant`) : 

* a `std::variant` containing the `VtInputKey` value `ESCAPE` ;
* a `std::variant` containing the printable character `[` ;
* a `std::variant` containing the `VtInputKey` value `ARROW_UP` ;

### VtInputFromCharacters give access to available data while still accepting characters for the current sequence

**given** VtInputFromCharacters has been reset

**when** VtInputFromCharacters is fed with the character sequence "\x1b[\x1b"

**then** VtInputFromCharacters still accept characters

**then** the VtInputFromCharacters will containing the following sequence of data (`std::variant`) : 

* a `std::variant` containing the `VtInputKey` value `ESCAPE` ;
* a `std::variant` containing the printable character `A` ;

### VtInputFromCharacters should fall back to single-character conversion when it is aborted

**given** VtInputFromCharacters has been reset and been fed with the character sequence "\x1b["

**when** VtInputFromCharacters is aborted

**then** VtInputFromCharacters does not accept characters anymore

**then** the VtInputFromCharacters will containing the following sequence of data (`std::variant`) : 

* a `std::variant` containing the `VtInputKey` value `ESCAPE` ;
* a `std::variant` containing the printable character `[` ;

### VtInputFromCharacters clear its internal state when it is reset

**given** VtInputFromCharacters has been reset and been fed with the character sequence "\x1b["

**when** VtInputFromCharacters is reset

**then** VtInputFromCharacters still accept characters

**then** the VtInputFromCharacters contains no data

**when** VtInputFromCharacters is fed with the single character `A`

**then** VtInputFromCharacters does not accept characters anymore

**then** the VtInputFromCharacters will return a `std::variant` containing `A`.


---
# sandbox

```cpp
class AsciiVtInputFromRawCharacters {
  public:
    bool canAppend() ; // true when more data is needed to obtain a VtInput, false when data is available.
    void append(char8_t) ; //feeds the converter
    void abort() ; // any appended characters will be made available
    bool hasData() ; //true as long as all the data has not be consumed (when finally there was nothing, all the chars can be retrieved)
    std::optional<AsciiVtInput> getData() ; //when there are non consumed data available, returns a non empty value
    void reset() ; //when all data has been retrieved
};
/* application */
/* BEGIN */
//given
DataSource<char8_t> rawCharacters;
AsciiVtInputFromRawCharacters converter;
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

