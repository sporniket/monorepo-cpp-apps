---
creation-date: 2026-05-29
copyright-years: 2026
copyright-authors: David SPORN
SPDX-License-Identifier: AGPL-3.0-or-later
---
# Programming rules for C++

## Preliminary statements

### I love Java

While C++ was my first crush on OOP, I find that Java is the best OOP language ever designed, so expect a fair bias toward the Java way of doing things.

### A byte is an octet

I never used CPU that have exotic memory models, in other words,

* I have deeply internalized that a _byte_ is an _octet_
* **Given** a struct S is made of two bytes, **when** one is given a pointer to such a struct, **then** it can be casted to a pointer to `uint16_t` with a value combining the two bytes depending on the endianness of the CPU.

```cpp
// let’s go up to 64-bits
struct Demo {
  uint8_t a;
  uint8_t b;
  uint8_t c;
  uint8_t d;
  uint8_t e;
  uint8_t f;
  uint8_t g;
  uint8_t h;
};

Demo value({0x12,0x34,0x56,0x78,0x9a,0xbc,0xde,0xf0});
Demo* ptr = &value;

uint8_t* byteArray = (uint8_t*)ptr; // contains 0x12,0x34,0x56,0x78,0x9a,0xbc,0xde,0xf0
// let’s say the CPU endianness is Little Endian
uint16_t* uint16Array = (uint16_t*)ptr; // contains 0x3412,0x7856,0xbc9a, 0xf0de
uint32_t* uint32Array = (uint32_t*)ptr; // contains 0x78563412, 0xf0debc9a
uint64_t* uint64Array = (uint64_t*)ptr; // contains 0xf0debc9a78563412

```
 
## Goals

* To Favor the standard library, unless I find a good reason to do differently.
* To follow the spirit of the [C++ Core Guidelines](https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md), unless I find a good reason to do differently.

## Syntax

### Prefer return by value

* It may trigger (Named) Return Value Optimisation.
* It is simpler to think and reason about.


## Types

### Use types with explicit size and signedness instead of implicite ones

**DON’T USE** : char, wchar, short, int, ...

**USE** : char8_t, uint8_t, ...

### Use char32_t and std::u32string for anything text

This is a refinement of the previous rule, enforcing the use of full width Unicode chars.

Exceptions:

* The project is restricted to support only US-ASCII characters (code 0 to 127)
* code leveraging system calls requiring char, wchar, etc...
* code tasked with I/O operations, and tasked encoding to/decoding from a given encoding charset, e.g. UTF-8, ISO-8859-1, CP1252, ...

### Design types with string values agnostically

If a type `T` has fields being strings, or uses strings in its methods : 

* Define a template `BasicT<...,class S>` with S a string type, e.g. `std::string`, `std::u32string`,...
* Define `T` as `BasicT<...,std::u32string>` ; implements the specifics of this alias as needed.
* Optionally, define `TAscii` as `Basic<...,std::string>` ; implements the specifics of this alias as needed.



### Return std::expected or std::optional instead of error codes

When designing an API for a task that MAY fail : 

* DO use `std::expected<T,E>` for public APIs.
* MAY use `std::optional<T>` in private APIs, when knowing which kind of error does not matter or won't be reported to anyone anyway.
* DON'T use a sentinel value to express OK/ERROR/WHICH ERROR. 

## Style

### Documentation comments for doxygen

**DOC.JavaStyle** Use Java style (`/**...*/` blocs with automatic brief.

DO NOT use single line comment after the element to document : I find it disruptive to watch for two very different patterns of comments.

e.g.

```cpp
/**
 * A short description ending with a dot.
 */
class SimplyDocumented{
  /**
   * A short description ending with a dot.
   * 
   * Having a short complimentary text still count for simple.
   *
   * Because it is not too tedious to maintain the leading '*'.
   */
  SimplyDocumented(){}

  /**
   * Functions are documented with parameters in the blocks.
   *
   * @param arg some arguments.
   */
  void doSomething(With arg);
};
```

**DOC.JavaStyle_Heavy** The Java style block is altered to mark a stronger visual separation when the documentation is long and/or include snippet of codes.

* The opening and closing markers use 49 ’*’ (thus 50 characters including the '/') and have the **same** indentation (the closing DOES NOT start with a supplemental whitespace).
* The body have no leading '*'.

e.g. (Let’s pretend that in the embedded code, code fences is `$$$` because of markdown-ception)

```cpp
/*************************************************
A finely crafted function.

This function has been devised to be typically used as following : 

$$$cpp
#include "the_header"

//... do not use block comments or all fall appart please

T someVar

void main() {
    callMe(&someVar);
}
$$$

*************************************************/
void callMe(T* maybe);
```

