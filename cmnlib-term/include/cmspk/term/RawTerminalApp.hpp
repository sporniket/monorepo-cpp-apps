// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 David SPORN
// ---
// This is part of **term library by sporniket**.
// A collection of utilities for writing terminal-hosted applications.
// ---

#ifndef __CMSPK__TERM__RAWTERMINALAPP_HPP__
#define __CMSPK__TERM__RAWTERMINALAPP_HPP__

#include <termios.h>

#include <string>

namespace cmspk::term {
// ================[ CODE BEGINS ]================
/**
 * Unicode string representing a CR+LF line end.
 */
const std::u32string RAW_ENDL(U"\r\n");
/**
 * US-ASCII string representing a CR+LF line end, to be used instead of std::endl.
 */
const std::string ASCII_RAW_ENDL("\r\n");

/**
 * Express whether the terminal screen should be cleared or not as some point.
 */
enum ClearScreenPolicy {
    /**
     * Leave the screen as is.
     */
    DO_NOTHING,
    /**
     * Clear the screen.
     */
    CLEAR_SCREEN
};

/**
 * Configuration structure for `RawTerminalApp`.
 */
struct RawTerminalAppOptions {
    /**
     * Whether the terminal screen should be cleared on startup.
     */
    ClearScreenPolicy clearScreenPolicyOnStartup;
    /**
     * Whether the terminal screen should be cleared on shutdown.
     */
    ClearScreenPolicy clearScreenPolicyOnShutdown;
};

/************************************************
 This class use RAII pattern to setup the terminal (termios struct) into raw mode (not buffered until pressing the `return` key, no interpreted
key-strokes, etc...).

It provide a `die(std::string)` utility for emergency exit of the program.

`RawTerminalApp` is a **non-copyable**, **non-moveable** type ; it is intended to be instancied at a static storage level outside of main and to stay there.

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
************************************************/
class RawTerminalApp {
  public:
    /**
     * Memorize the initial terminal settings, switch to raw terminal mode.
     *
     * @param options settings for the parametrable behaviours.
     */
    RawTerminalApp(RawTerminalAppOptions options);

    /**
     * restore the terminal settings as it was memorized at instantiation time.
     */
    ~RawTerminalApp();
    //  TODO rule of zero : prevent copying/moving

    /**
     * Deleted copy operation, this class is not copyable.
     */
    RawTerminalApp(const RawTerminalApp&) = delete;

    /**
     * Deleted copy operator, this class is not copyable.
     */
    RawTerminalApp& operator=(const RawTerminalApp&) = delete;

    /**
     * Deleted move operation, this class is not movable.
     */
    RawTerminalApp(RawTerminalApp&&) = delete;

    /**
     * Deleted move operator, this class is not movable.
     */
    RawTerminalApp& operator=(RawTerminalApp&&) = delete;

    /**
     * Interrupt the application with an error message to display.
     *
     * @param message the message to display, provided it is not empty.
     */
    void die(const std::string& message);

  private:
    /**
     * Storage for the settings before switching to raw mode.
     */
    struct termios origTermios;

    /**
     * Settings for the parametrable behaviours.
     */
    struct RawTerminalAppOptions options;

    /**
     * Storage for an error message to display after calling `std::exit()`.
     */
    std::string message;

    void printMessage();
};

// ================[ END OF CODE ]================
}  // namespace cmspk::term
#endif
