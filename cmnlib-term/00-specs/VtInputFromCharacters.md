---
creation-date: 2026-06-12
copyright-years: 2026
copyright-authors: David SPORN
SPDX-License-Identifier: AGPL-3.0-or-later
since: 0.0.2
---

# VtInputFromCharacters

**The term "VT" is "Virtual Verminal"**.

Converts a sequence of characters (`char8_t`) into a sequence of virtual terminal inputs.

Typical application : 

```cpp
// TODO make pop() returns an std::optional (empty when cannot pop)
// TODO make push() returns an std::expected<void, VtInputFromCharactersError>

// GIVEN
VtInputFromCharacters converter ;
converter.reset() ;
static_assert(converter.canPop() == false, "converter.canPop() should be false after reset")
static_assert(converter.canPush() == false, "converter.canPush() should be true after reset")

// some inputs only need a single char
converter.push('H') ; // converter.canPush() is false, converter.canPop() is true
if (converter.canPop()) {
  VtInput result = converter.pop() ; // contains the char8_t 'H'
}

// some input requires more
converter.push(27); // converter.canPop() is false
converter.push('['); // converter.canPop() is false
converter.push('A'); // converter.canPop() is true
if (converter.canPop()) {
  VtInput result = converter.pop() ; // contains a VtInputKey VtInputKey::ARROW_UP
}

// It can be interrupted, in this case multi-character sequence matching is interrupted
converter.push(27); // converter.canPop() is false
converter.push('['); // converter.canPop() is false
converter.abort(); // converter.canPop() is true
if (converter.canPop()) {
  VtInput result = converter.pop() ; // contains a VtInputKey VtInputKey::ESCAPE
  VtInput result2 = converter.pop() ; // contains a char8_t '['
}
```

## Requirements

* [Specfication guidelines](../../README--specification-guidelines.md)
* [VtInput](./VtInput.md)


## Behaviours

### It should return a virtual terminal input NONE on reading an octet with value 0 or in range 28 to 32

**given** VtInputFromCharacters has been reset

**when** VtInputFromCharacters is fed with the single character zero (`0x00`) or a character in range 28 to 32

**then** VtInputFromCharacters does not accept characters anymore

**then** the VtInputFromCharacters will return a `std::variant` containing a `VtInputNull`.

### It should return printable characters

**For each characters _Chr_ (`char8_t`) in range(32,256) excluding 127**

**given** VtInputFromCharacters has been reset

**when** VtInputFromCharacters is fed with the single character _Chr_

**then** VtInputFromCharacters does not accept characters anymore

**then** the VtInputFromCharacters will return a `std::variant` containing _Chr_.

### It should return keys on recognizing key sequences

**For each enum value _K_ in `VtInputKey`**

**given** VtInputFromCharacters has been reset

**when** VtInputFromCharacters is fed with a character sequence that should be recognized as _K_

**then** VtInputFromCharacters does not accept characters anymore

**then** the VtInputFromCharacters will return a `std::variant` containing _K_.

### It should return Vt report on recognizing cursor position report

**given** VtInputFromCharacters has been reset

**when** VtInputFromCharacters is fed with the character sequence "\x1b[24;80R"

**then** VtInputFromCharacters does not accept characters anymore

**then** the VtInputFromCharacters will return a `std::variant` containing the `VtInputReport` of type `CURSOR_POSITION`, with 2 arguments "24" and "80".

### It should fall back to single-character conversion when a sequence is finally not recognized

**given** VtInputFromCharacters has been reset

**when** VtInputFromCharacters is fed with the character sequence "\x1bA"

**then** VtInputFromCharacters does not accept characters anymore

**then** the VtInputFromCharacters will containing the following sequence of data (`std::variant`) : 

* a `std::variant` containing the `VtInputKey` value `ESCAPE` ;
* a `std::variant` containing the printable character `A` ;

### It should start a new sequence match when the current sequence is broken by the next character

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

### It should fall back to single-character conversion when it is aborted

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


## Technical details

---
# sandbox

```cpp
class AsciiVtInputFromRawCharacters {
  public:
    bool canAppend() ; // true when more data is needed to obtain a VtInput, false when data is available.
    void append(char8_t) ; //feeds the converter
    void abort() ; // any appended characters will be made available
    bool canPop() ; //true as long as all the data has not be consumed (when finally there was nothing, all the chars can be retrieved)
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
while(converter.canPop()) {
  return converter.getData() ; // imagine the host call in a loop.
}
if(ioError is not empty) {
  return ioError.value()
}
converter.reset();
/* END */

```

