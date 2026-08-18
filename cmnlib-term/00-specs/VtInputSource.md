---
creation-date: 2026-06-12
copyright-years: 2026
copyright-authors: David SPORN
SPDX-License-Identifier: AGPL-3.0-or-later
since: 0.0.2
---

# VtInputSource

**The term "VT" is "Virtual Terminal".**

As the name implies, an data source of `VtInput`.

Typical application : 

```cpp
static std::shared<DataSource<char8_t>> usuallyStandardInput ;
static VtInputSource myInput(usuallyStandardInput) ;
static bool running = true;

// ... main loop
while(running) {
  std::expected<VtInput,BasicIoError<char8_t>> nextInput = myInput.next() ;
  if (!nextInput) { continue ; }
  
  //... process the actual value of myInput
}

```

Under the hood, it reads from a provided data source of raw characters `char8_t` and manages a `VtInputFromCharacters`.

## Requirements

* [Specfication guidelines](../../README--specification-guidelines.md)
* [VtInput](./VtInput.md)
* [VtInputFromCharacters](./VtInputFromCharacters.md)
* [DataSource](../../cmnlib-io/00-specs/DataSource.md)


## Behaviours

> TO BE REWRITTEN
>
> * It should process single-octet values (1~26, 32~255 except 127)
> * It should process multi-octets sequences (e.g. page up, cursor location report)
> * It should stop multi-octets sequence matching when the datasource cannot provides the next character while processing such sequence (i.e. a datasource always have all the octets available for a multi-octets sequence)
>
> START FROM HERE

### It should return a Vt null on reading the zero octet

**given** a character data source that will return zero.

**when** reading the next VtInput with `next()`

**then** the VtInputSource will return a `std::variant` containing a `VtInputNull`.

### It should return printable characters

**For each characters `Chr` (`char8_t`) in range(32,256) excluding 127**

**given** a data source of characters that will return `Chr`

**when** reading the next VtInput with `next()`

**then** the VtInputSource will return a `std::variant` containing the char8_t `Chr`.

### It should return keys on recognizing key sequences

**For each enum value `K` in `VtInputKey`**

**given** a character data source that will return the Vt sequence of characters that should be recognized as `Ks`

**when** reading the next VtInput with `next()`

**then** the VtInputSource will return a `std::variant` containing the `VtInputKey` `Ks`.

### It should return Vt report on recognizing cursor position report

**given** a character data source that will return the sequence "\x1b[24;80R"

**when** reading the next VtInput with `next()`

**then** the VtInputSource will return a `std::variant` containing the `VtInputReport` of type `CURSOR_POSITION`, with 2 arguments "24" and "80".

### It should buffer single-octet Vt inputs when a sequence is interrupted by an error.

**given** a character data source that will return the sequence "\x1b[2" and then an IoError `END_OF_DATA`

**when** reading the next VtInput in a loop with `next()` with a stop when getting an IoError

**then** the VtInputSource will return the following sequence of `std::variant` before getting an IoError `END_OF_DATA` : 

* a `std::variant` containing the `VtInputKey` value `ESCAPE` ;
* a `std::variant` containing the printable character `[` ;
* a `std::variant` containing the printable character `2` ;

### It should not lose data when a new sequence interrupt a partial sequence of a multi-octets Vt input

**given** a character data source that will return the sequence "\x1b[2\x1b[A"

**when** reading the next VtInput four times

**then** the VtInputSource will return the following sequence of `std::variant` : 

* a `std::variant` containing the `VtInputKey` value `ESCAPE` ;
* a `std::variant` containing the printable character `[` ;
* a `std::variant` containing the printable character `2` ;
* a `std::variant` containing the `VtInputKey` value `ARROW_UP` ;

## Technical details


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

