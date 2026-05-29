// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 David SPORN
// ---
// This is a minimal demo of how to use cmspk::term::RawTerminalApp, its
// lifecycle and its emergency app termination `die(message)`.
// ---

// standard includes
#include <iostream>

// other includes
#include "cmspk/term/RawTerminalApp.hpp"

// By virtue of the RAII pattern and static storage,
// the terminal raw mode will be enabled before entering `main()`
static cmspk::term::RawTerminalApp app({.clearScreenPolicyOnStartup = cmspk::term::ClearScreenPolicy::DO_NOTHING,
                                        .clearScreenPolicyOnShutdown = cmspk::term::ClearScreenPolicy::DO_NOTHING});

int main() {
    std::cout                                                //
        << "will be cleared" << cmspk::term::ASCII_RAW_ENDL  //
        << "will be flushed before and" << cmspk::term::ASCII_RAW_ENDL << "after destruction of `app`.";

    // errno is not set, so expect to read "message of premature quit: Success"
    app.die("message of premature quit");

    std::cout << "unreachable statement";
}
// after main, app is destroyed, which restores terminal 'cooked' mode and
// clears the screen before printing the error message given with `app.die(...)`.
