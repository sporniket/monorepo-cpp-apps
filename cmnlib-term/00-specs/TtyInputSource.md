---
creation-date: 2026-06-12
copyright-years: 2026
copyright-authors: David SPORN
SPDX-License-Identifier: AGPL-3.0-or-later
since: 0.0.2
---

# TtyInputSource

## Assumptions and definitions

* **The term _byte_ is banned**, unless the design is intently targetting the smallest addressable unit of the processing unit ; most of the time, it is an octet, but it is mere coincidence ; e.g. read https://software-dl.ti.com/ccs/esd/documents/c2000_byte-accesses-with-the-c28x-cpu.html
* As a result of the previous statement, **this design is restricted to architectures where a byte is an octet**.
* A **character**, as read from low level stdin, is a 8-bits, unsigned value, a.k.a. an octet.
* We will use `char8_t` as the character type.

## Overview

> **Note :** although this specification use "TTY" to speak of serial terminals, it will be expected to be
> inside a **virtual terminal**.

An adapter of a character data source that presents itself as a data source of tty inputs, that are

* either a _report_, usually after sending an output request to the TTY output, e.g. a report on the cursor position;
* or a _key stroke_, either encoded as a single char (1 to 27, 127), or as an escape sequence (`"\x1b[A"` for cursor up)
* or the null byte (zero), that will be ignored
* otherwise, this is a printable character, or the start of a multi-octets character, depending of the encoding.

## Technical details

* Additional constants
* Additional types
* TtyInputSource features

### Additional constants

|type|name|brief|
|---|---|---|
|enum `TtyInputKey`|`CTRL_A` (=1)|Key combination : `CTRL` + `A`|
|enum `TtyInputKey`|`CTRL_B`|Key combination : `CTRL` + `B`|
|enum `TtyInputKey`|`CTRL_C`|Key combination : `CTRL` + `C`|
|enum `TtyInputKey`|`CTRL_D`|Key combination : `CTRL` + `D`|
|enum `TtyInputKey`|`CTRL_E`|Key combination : `CTRL` + `E`|
|enum `TtyInputKey`|`CTRL_F`|Key combination : `CTRL` + `F`|
|enum `TtyInputKey`|`CTRL_G`|Key combination : `CTRL` + `G`|
|enum `TtyInputKey`|`CTRL_H`|Key combination : `CTRL` + `H`|
|enum `TtyInputKey`|`HTAB`|`Horizontal tabulation` key|
|enum `TtyInputKey`|`CTRL_J`|Key combination : `CTRL` + `J`|
|enum `TtyInputKey`|`CTRL_K`|Key combination : `CTRL` + `K`|
|enum `TtyInputKey`|`CTRL_L`|Key combination : `CTRL` + `L`|
|enum `TtyInputKey`|`RETURN`|`Carriage return` key|
|enum `TtyInputKey`|`CTRL_N`|Key combination : `CTRL` + `N`|
|enum `TtyInputKey`|`CTRL_O`|Key combination : `CTRL` + `O`|
|enum `TtyInputKey`|`CTRL_P`|Key combination : `CTRL` + `P`|
|enum `TtyInputKey`|`CTRL_Q`|Key combination : `CTRL` + `Q`|
|enum `TtyInputKey`|`CTRL_R`|Key combination : `CTRL` + `R`|
|enum `TtyInputKey`|`CTRL_S`|Key combination : `CTRL` + `S`|
|enum `TtyInputKey`|`CTRL_T`|Key combination : `CTRL` + `T`|
|enum `TtyInputKey`|`CTRL_U`|Key combination : `CTRL` + `U`|
|enum `TtyInputKey`|`CTRL_V`|Key combination : `CTRL` + `V`|
|enum `TtyInputKey`|`CTRL_W`|Key combination : `CTRL` + `W`|
|enum `TtyInputKey`|`CTRL_X`|Key combination : `CTRL` + `X`|
|enum `TtyInputKey`|`CTRL_Y`|Key combination : `CTRL` + `Y`|
|enum `TtyInputKey`|`CTRL_Z`|Key combination : `CTRL` + `Z`|
|enum `TtyInputKey`|`ESCAPE`|`ESCAPE` key|
|enum `TtyInputKey`|`ARROW_UP`|Arrow key : up |
|enum `TtyInputKey`|`ARROW_DOWN`|Arrow key : down |
|enum `TtyInputKey`|`ARROW_LEFT`|Arrow key : left |
|enum `TtyInputKey`|`ARROW_RIGHT`|Arrow key : right |
|enum `TtyInputReportType`|`CURSOR_POSITION`|A report containing the cursor position ; e.g "\x1b[24;80R"|
|const `TtyInputNull`|`NULL_CHAR`|a constant to easily return a `TtyInputNull`|

### Additional types

* `TtyInputNull` : an empty type, to be returned when the datasource get zero (0), which should not happen in a terminal input.
* `TtyInputReport` : a report, like the terminal dimensions.

```cpp
struct TtyInputNull {};

template<class C>
struct TtyInputReport {
  TtyInputReportType type,
  std::vector<std::u8string> parameters
};
```

### TtyInputSource features

A `TtyInputSource` is plugged to a character data source. 

When instructed to return the next data, it reads as much data needed in order to match one of the 4 possible outcomes.

When trying to read a multi-octets sequence, and when it matches, then all the octets of the sequence are _consumed_. The next data will read the data source.

When trying to read a multi-octets sequence, and when it does not matches, then all the octets of the sequence are _memorized_, and it tries to match another kind of tty input using those memorized octets.

The recommanded order of matching is the following : 

* the null octet (zero)
* a single-octet key stroke
* a tty report
* a multi-octets key stroke 

## Tests

### TtyInputSource should return a tty null

**given** a character data source that will return zero.

**when** reading the next TtyInput with `next()`

**then** the TtyInputSource will return a `std::variant` containing a `TtyInputNull`.

### TtyInputSource should return printable characters

**For each characters `Chr` (`char8_t`) in range(32,256) excluding 127**

**given** a data source of characters that will return `Chr`

**when** reading the next TtyInput with `next()`

**then** the TtyInputSource will return a `std::variant` containing the char8_t `Chr`.

### TtyInputSource should return keys

**For each enum value `K` in `TtyInputKey`**

**given** a character data source that will return the TTY sequence of characters that should be recognized as `Ks`

**when** reading the next TtyInput with `next()`

**then** the TtyInputSource will return a `std::variant` containing the `TtyInputKey` `Ks`.

### TtyInputSource should return cursor position report

**given** a character data source that will return the sequence "\x1b[24;80R"

**when** reading the next TtyInput with `next()`

**then** the TtyInputSource will return a `std::variant` containing the `TtyInputReport` of type `CURSOR_POSITION`, with 2 arguments "24" and "80".

### TtyInputSource should reuse unconsumed reads from the character data source

**given** a character data source that will return the sequence "\x1b[2" and then an IoError `END_OF_DATA`

**when** reading the next TtyInput in a loop with `next()` with a stop when getting an IoError

**then** the TtyInputSource will return the following sequence of `std::variant` before getting an IoError `END_OF_DATA` : 

* a `std::variant` containing the `TtyInputKey` value `ESCAPE` ;
* a `std::variant` containing the printable character `[` ;
* a `std::variant` containing the printable character `2` ;

---
# sandbox

```cpp
enum class TtyInputKey {
  CTRL_A, CTRL_B,..., CTRL_Z, // mapped to 1..26 ; CTRL_I/CTRL_M are replaced by HTAB/RETURN
  ARROW_UP,ARROW_DOWN,ARROW_LEFT,ARROW_RIGHT, // in the order A/B/C/D of the escape sequence
  //...
};

enum class TtyInputReportType {
  TERM_DIMENSION, // name to confirm
  //...
}

struct TtyInputReport {
  TtyInputReportType,
  std::vector<std::variant<int64_t, std::u8string>>
};

// Tty works octet by octet. Decoding multi-octets encoding comes later and is out of scope of TtyInput.
using TtyInput = std::variant<char8_t, TtyInputKey, TtyReport>

class TtyInputSource : public cmspk::io::DataSource<TtyInput> {
  public:
  TtyInputSource(/*FILE HANDLER*/ inputSource):inputSource(inputSource){};
  // add rule of five
  virtual ~TtyInputSource() = default;
  // ...
  
  // TODO implement DataSource interface, that process the input

  private:
  /*FILE HANDLER, e.g. STDIN*/ inputSource ;
}


// typical usage
TtyInputSource stdin(STDIN);
bool running = true; // crude state machine

void main() {

  while(running) {
    std::optionnal<TtyInput> input = stdin.read() ;
    if (!input) continue;
    
    //process input
    if (*input.hold<TtyInputReport>()) { /* react to report...*/ }
    else if (*input.hold<TtyInputKey>()) { /* react to key...*/ }
    else if (*input.hold<char8_t>()) { /* usually send to decoder...*/ }
    else {/* handle unknown feature */}
    
  }

}

```

