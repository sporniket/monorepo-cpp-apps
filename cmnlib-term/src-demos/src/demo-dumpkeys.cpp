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
#include "cmspk/term/VtInputSource.hpp"

// By virtue of the RAII pattern and static storage,
// the terminal raw mode will be enabled before entering `main()`
static cmspk::term::RawTerminalApp app({.clearScreenPolicyOnStartup = cmspk::term::ClearScreenPolicy::DO_NOTHING,
                                        .clearScreenPolicyOnShutdown = cmspk::term::ClearScreenPolicy::CLEAR_SCREEN});

// BEGIN To be moved -- encapsulate read into DataSource
#include <functional>

#include "cmspk/io/DataSource.hpp"
class AsciiCharSourceFromStdRead : public cmspk::io::BasicDataSource<char8_t, char8_t> {
  public:
    AsciiCharSourceFromStdRead(int fileDescriptorId, std::function<ssize_t(int, void*, size_t)> readImpl = read)
        : fileDescriptorId(fileDescriptorId), readImpl(readImpl) {}
    virtual std::expected<char8_t, cmspk::io::IoErrorAscii> next() {
        if (irrecoverable) {
            return std::unexpected(AsciiCharSourceFromStdRead::irrecoverable_error());
        }
        int result = readImpl(fileDescriptorId, &c, 1);
        int savedErrno = errno;
        if (result < 0) {
            if (savedErrno != EAGAIN && savedErrno != EWOULDBLOCK) {
                irrecoverable = true;
                return std::unexpected(AsciiCharSourceFromStdRead::irrecoverable_error());
            }
            return std::unexpected(AsciiCharSourceFromStdRead::not_ready_error());
        } else if (result == 0) {
            return std::unexpected(AsciiCharSourceFromStdRead::not_ready_error());
        }
        return c;
    }
    virtual bool hasNext() { return !irrecoverable; }

    virtual ~AsciiCharSourceFromStdRead() {}
    /**
     * Copy operation (rule of 5).
     */
    AsciiCharSourceFromStdRead(const AsciiCharSourceFromStdRead&) = default;

    /**
     * Copy operator (rule of 5).
     */
    AsciiCharSourceFromStdRead& operator=(const AsciiCharSourceFromStdRead&) = default;

    /**
     * Move operation (rule of 5).
     */
    AsciiCharSourceFromStdRead(AsciiCharSourceFromStdRead&&) = default;

    /**
     * Move operator (rule of 5).
     */
    AsciiCharSourceFromStdRead& operator=(AsciiCharSourceFromStdRead&&) = default;

  private:
    int fileDescriptorId;
    char8_t c;
    bool irrecoverable = false;
    std::function<ssize_t(int, void*, size_t)> readImpl;
    static const cmspk::io::IoErrorAscii irrecoverable_error() { return {.type = cmspk::io::IoErrorType::BAD, .message = u8"bad.state", .details = {}}; }
    static const cmspk::io::IoErrorAscii not_ready_error() { return {.type = cmspk::io::IoErrorType::NOT_READY, .message = u8"try.later", .details = {}}; }
};
// END To be moved

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

static bool isRunning = true;  // crude state machine

void notifyQuit() { isRunning = false; }

void onUnknownVtInput(cmspk::term::VtInputUnknown vtin) {
    char8_t rawValue = vtin.rawValue;
    std::cout << "Unknown \t(" << (uint16_t)rawValue << ")" << cmspk::term::ASCII_RAW_ENDL;
}

void onCharVtInput(char8_t vtin) {
    if (vtin >= 32 && vtin < 128) {
        std::cout << "Printable :\t" << (uint16_t)vtin << " ('" << (char)vtin << "') " << cmspk::term::ASCII_RAW_ENDL;
    } else {
        std::cout << "Printable :\t" << (uint16_t)vtin << cmspk::term::ASCII_RAW_ENDL;
    }
    if (vtin == (char8_t)'q') {
        notifyQuit();
    }
}

void onKeyVtInput(cmspk::term::VtInputKey vtin) {
    if (cmspk::term::vt_input_key_names_ascii.contains(vtin)) {
        std::cout << "Key :\t" << (const char*)cmspk::term::vt_input_key_names_ascii.at(vtin).c_str() << cmspk::term::ASCII_RAW_ENDL;
    } else {
        std::cout << "Unamed key :\t" << (uint16_t)vtin << cmspk::term::ASCII_RAW_ENDL;
    }
}

int main() {
    cmspk::term::VtInputSource source(std::unique_ptr<cmspk::io::BasicDataSource<char8_t, char8_t>>(new AsciiCharSourceFromStdRead(STDIN_FILENO)));
    while (isRunning) {
        std::expected<cmspk::term::VtInput, cmspk::io::IoErrorAscii> nextInput = source.next();
        if (nextInput) {
            cmspk::term::VtInput in = (*nextInput);
            if (std::holds_alternative<cmspk::term::VtInputUnknown>(in)) {
                onUnknownVtInput(std::get<cmspk::term::VtInputUnknown>(in));
            } else if (std::holds_alternative<char8_t>(in)) {
                onCharVtInput(std::get<char8_t>(in));
            } else if (std::holds_alternative<cmspk::term::VtInputKey>(in)) {
                onKeyVtInput(std::get<cmspk::term::VtInputKey>(in));
            }
        } else {
            cmspk::io::IoErrorAscii error = nextInput.error();
            if (error.type != cmspk::io::IoErrorType::NOT_READY) {
                std::cout << "I/O Error : " << (const char*)error.message.c_str() << cmspk::term::ASCII_RAW_ENDL;
                app.die("read");
            }
        }
    }
    return 0;
}
// after main, app is destroyed, which restores terminal 'cooked' mode and
// clears the screen before printing the error message given with `app.die(...)`.
