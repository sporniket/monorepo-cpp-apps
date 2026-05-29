// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 David SPORN
// ---
// This is demo of how to use cmspk::term::RawTerminalApp that also demonstrate
// that we are effectively in raw mode. This is done by outputing raw bytes from
// stdin.
// ---

// standard includes
#include <unistd.h>

#include <iostream>
#include <string>

// other includes
#include "cmspk/term/RawTerminalApp.hpp"

// By virtue of the RAII pattern and static storage,
// the terminal raw mode will be enabled before entering `main()`
static cmspk::term::RawTerminalApp app({.clearScreenPolicyOnStartup = cmspk::term::ClearScreenPolicy::DO_NOTHING,
                                        .clearScreenPolicyOnShutdown = cmspk::term::ClearScreenPolicy::CLEAR_SCREEN});

bool isPrintable(char c) {
    if (iscntrl(c)) {
        return false;
    }
    if (((unsigned char)c) < 127) {
        // Essentially, only print US-ASCII chars expect character 127 that is "backspace".
        return true;
    }
    return false;
}

int main() {
    while (1) {
        char c = '\0';
        if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) {
            app.die("read");
        }
        if (0 == c) {
            continue;
        }
        if (isPrintable(c)) {
            std::cout << ((unsigned int)c & 0xff) << " ('" << c << "')" << cmspk::term::ASCII_RAW_ENDL;
        } else {
            std::cout << ((unsigned int)c & 0xff) << cmspk::term::ASCII_RAW_ENDL;
        }
        if (c == 'q') break;
    }
    return 0;
}
// after main, app is destroyed, which restores terminal 'cooked' mode and
// clears the screen before printing the error message given with `app.die(...)`.
