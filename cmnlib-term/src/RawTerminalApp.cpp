// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 David SPORN
// ---
// This is part of **term library by sporniket**.
// A collection of utilities for writing terminal-hosted applications.
// ---
#include "cmspk/term/RawTerminalApp.hpp"

#include <termios.h>
#include <unistd.h>

namespace cmspk::term {
// ================[ CODE BEGINS ]================
const std::string CLEAR_SCREEN_SEQUENCE("\x1b[2J\x1b[H");
const std::string DIE_EPILOG("\n");

RawTerminalApp::RawTerminalApp(RawTerminalAppOptions options) : options(options) {
    if (tcgetattr(STDIN_FILENO, &origTermios) == -1) {
        die("tcgetattr");
    }

    struct termios raw = origTermios;
    cfmakeraw(&raw);

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
        die("tcsetattr");
    }

    if (ClearScreenPolicy::CLEAR_SCREEN == options.clearScreenPolicyOnStartup) {
        write(STDOUT_FILENO, CLEAR_SCREEN_SEQUENCE.c_str(), CLEAR_SCREEN_SEQUENCE.size());  //  TODO check effective write length and die if there is a problem.
    }
}

RawTerminalApp::~RawTerminalApp() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &origTermios) == -1) {
        message = "tcsetattr";
        printMessage();
        std::exit(1);  // ? possible ?
    }
    printMessage();
}

void RawTerminalApp::die(const std::string& message) {
    this->message = message;
    std::exit(1);
}

void RawTerminalApp::printMessage() {
    if (ClearScreenPolicy::CLEAR_SCREEN == options.clearScreenPolicyOnShutdown) {
        write(STDOUT_FILENO, CLEAR_SCREEN_SEQUENCE.c_str(), CLEAR_SCREEN_SEQUENCE.size());
    }
    if (message.size() > 0) {
        perror(message.c_str());
    }
}

// ================[ END OF CODE ]================
}  // namespace cmspk::term
