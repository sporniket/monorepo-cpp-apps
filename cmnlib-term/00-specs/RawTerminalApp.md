---
creation-date: 2026-05-14
copyright-years: 2026
copyright-authors: David SPORN
SPDX-License-Identifier: AGPL-3.0-or-later
---

# RawTerminalApp

## Overview

This class use RAII pattern to setup the terminal (termios struct) into raw mode (not buffered until pressing the `return` key, no interpreted key-strokes, etc...).

It provide a `die(std::string)` utility for emergency exit of the program.

The typical usage would look like :

```cpp
#include "cmspk/term/RawTerminalApp.hpp"

static cmspk::term::RawTerminalApp app({.clearScreenPolicyOnStartup = cmspk::term::ClearScreenPolicy::DO_NOTHING,
                                        .clearScreenPolicyOnShutdown = cmspk::term::ClearScreenPolicy::CLEAR_SCREEN});

// other stuff...
static running = true; // crude app lifecycle state

int main(int argc, char* argv[]) {
    while(running) {
        // ... do stuff
        if (some_error_condition()) {
            die("got some error");
        }
        // ... continue doing stuff
        if (the_user_triggered_exit_command()) {
            running = false;  // the end.
        }
    }
}
```

## Technical details

* Additional constants
* Additional types
* RawTerminalApp features

### Additional constants

|type|name|brief|
|---|---|---|
|`std::u32string`|`RAW_ENDL`|Unicode string representing a CR+LF line end|
|`std::string`|`ASCII_RAW_ENDL`|US-ASCII string representing a CR+LF line end, to be used instead of std::endl|
|enum `ClearScreenPolicy`|`DO_NOTHING`|As clear screen policy option value, tell to NOT clear the screen|
|enum `ClearScreenPolicy`|`CLEAR_SCREEN`|As clear screen policy option value, tell to clear the screen|

### Additional types

* **Structure** `RawTerminalAppOptions`, to influence the behaviour of `RawTerminalApp`.

```cpp
struct RawTerminalAppOptions {
    ClearScreenPolicy clearScreenPolicyOnStartup;
    ClearScreenPolicy clearScreenPolicyOnShutdown;
};
```

### RawTerminalApp features

`RawTerminalApp` is a **non-copyable**, **non-moveable** type ; it is intended to be instancied at a static storage level outside of main and to stay there.

```cpp
class RawTerminalApp {
  public:
    RawTerminalApp(RawTerminalAppOptions options);
    ~RawTerminalApp();
    void die(const std::string& message);
};
```

|Method|Brief|
|---|---|
|Constructor|Backup the terminal settings, and change to raw mode|
|Destructor|Restore the terminal settings, and optionnaly print an error message|
|`die(...)`|Set the provided string as the error message and call `std::exit()`|


## Manual tests

> It seems I could not write automated test, as `tcgetattr(...)` and `tcsetattr(...)` were not working in the test.

### RawTerminalApp manages the termios structure

**Using** the `demo-dumpkeys` demonstration program,

**Then** the key code `Ctrl+M` is received as character code 13 (carriage return).

**Then** the key code `Ctrl+C` does not interrupt the program.

**Then** pressing `q` quit the program, clears the screen and restore normal terminal settings.

### RawTermApp.die(...) exits the program and prints the given message

**Using** the `demo-bare` demonstration program.

**Then** one should observe the following output : 

```
[shell prompt]$ ./build/src-demos/demo-bare 
will be cleared
will be flushed before and
message of premature quit: Success
after destruction of `app`.[shell prompt]$
```

