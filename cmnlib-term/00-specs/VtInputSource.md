---
creation-date: 2026-06-12
copyright-years: 2026
copyright-authors: David SPORN
SPDX-License-Identifier: AGPL-3.0-or-later
since: 0.0.2
---

# VtInputSource

## Assumptions and definitions

* **The term "VT" is "Virtual Verminal"**.
* **The term _byte_ is banned**, unless the design is intently targetting the smallest addressable unit of the processing unit ; most of the time, it is an octet, but it is mere coincidence ; e.g. read https://software-dl.ti.com/ccs/esd/documents/c2000_byte-accesses-with-the-c28x-cpu.html
* As a result of the previous statement, **this design is restricted to architectures where a byte is an octet**.
* A **character**, as read from low level stdin, is a 8-bits, unsigned value, a.k.a. an octet.
* We will use `char8_t` as the character type.
* If possible, **supports 32-bits CPUs** 


## Require

* [VtInput](./VtInput.md)
* [VtInputFromCharacters](./VtInputFromCharacters.md)
* [DataSource](../../cmnlib-io/00-specs/DataSource.md)

## Overview

An adapter of a character data source that presents itself as a data source of Vt inputs.

## Technical details

* VtInputSource features

### VtInputSource features

A `VtInputSource` is plugged to a character data source. 

When instructed to return the next data, it reads as much data needed in order to match one of the 4 possible outcomes.

When trying to read a multi-octets sequence, and when it matches, then all the octets of the sequence are _consumed_. The next data will read the data source.

When trying to read a multi-octets sequence, and when it does not matches, then all the octets before the last one are converted to single-octet Vt input and buffered, while the last octet is either a single-octet Vt input, or the first octet of a potential multi-octets Vt input ; in the former case the last octet is converted to a single-octet Vt input and buffered after the others Vt inputs ; in the later case, a new attempt at reading a multi-octet Vt input is performed.

## Tests

### VtInputSource should return a Vt null on reading the zero octet

**given** a character data source that will return zero.

**when** reading the next VtInput with `next()`

**then** the VtInputSource will return a `std::variant` containing a `VtInputNull`.

### VtInputSource should return printable characters

**For each characters `Chr` (`char8_t`) in range(32,256) excluding 127**

**given** a data source of characters that will return `Chr`

**when** reading the next VtInput with `next()`

**then** the VtInputSource will return a `std::variant` containing the char8_t `Chr`.

### VtInputSource should return keys on recognizing key sequences

**For each enum value `K` in `VtInputKey`**

**given** a character data source that will return the Vt sequence of characters that should be recognized as `Ks`

**when** reading the next VtInput with `next()`

**then** the VtInputSource will return a `std::variant` containing the `VtInputKey` `Ks`.

### VtInputSource should return Vt report on recognizing cursor position report

**given** a character data source that will return the sequence "\x1b[24;80R"

**when** reading the next VtInput with `next()`

**then** the VtInputSource will return a `std::variant` containing the `VtInputReport` of type `CURSOR_POSITION`, with 2 arguments "24" and "80".

### VtInputSource should buffer single-octet Vt inputs when a sequence is interrupted by an error.

**given** a character data source that will return the sequence "\x1b[2" and then an IoError `END_OF_DATA`

**when** reading the next VtInput in a loop with `next()` with a stop when getting an IoError

**then** the VtInputSource will return the following sequence of `std::variant` before getting an IoError `END_OF_DATA` : 

* a `std::variant` containing the `VtInputKey` value `ESCAPE` ;
* a `std::variant` containing the printable character `[` ;
* a `std::variant` containing the printable character `2` ;

### VtInputSource should not lose data when a new sequence interrupt a partial sequence of a multi-octets Vt input

**given** a character data source that will return the sequence "\x1b[2\x1b[A"

**when** reading the next VtInput four times

**then** the VtInputSource will return the following sequence of `std::variant` : 

* a `std::variant` containing the `VtInputKey` value `ESCAPE` ;
* a `std::variant` containing the printable character `[` ;
* a `std::variant` containing the printable character `2` ;
* a `std::variant` containing the `VtInputKey` value `ARROW_UP` ;

---
# sandbox

```cpp
enum class VtInputKey {
  CTRL_A, CTRL_B,..., CTRL_Z, // mapped to 1..26 ; CTRL_I/CTRL_M are replaced by HTAB/RETURN
  ARROW_UP,ARROW_DOWN,ARROW_LEFT,ARROW_RIGHT, // in the order A/B/C/D of the escape sequence
  //...
};

enum class VtInputReporVtpe {
  TERM_DIMENSION, // name to confirm
  //...
}

struct VtInputReport {
  VtInputReporVtpe,
  std::vector<std::variant<int64_t, std::u8string>>
};

// Vt works octet by octet. Decoding multi-octets encoding comes later and is out of scope of VtInput.
using VtInput = std::variant<char8_t, VtInputKey, VtReport>

class VtInputSource : public cmspk::io::DataSource<VtInput> {
  public:
  VtInputSource(/*FILE HANDLER*/ inputSource):inputSource(inputSource){};
  // add rule of five
  virtual ~VtInputSource() = default;
  // ...
  
  // TODO implement DataSource interface, that process the input

  private:
  /*FILE HANDLER, e.g. STDIN*/ inputSource ;
}


// typical usage
VtInputSource stdin(STDIN);
bool running = true; // crude state machine

void main() {

  while(running) {
    std::optionnal<VtInput> input = stdin.read() ;
    if (!input) continue;
    
    //process input
    if (*input.hold<VtInputReport>()) { /* react to report...*/ }
    else if (*input.hold<VtInputKey>()) { /* react to key...*/ }
    else if (*input.hold<char8_t>()) { /* usually send to decoder...*/ }
    else {/* handle unknown feature */}
    
  }

}

```

