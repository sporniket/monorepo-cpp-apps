// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 David SPORN
// ---
// This program demonstrates the use of :
//
// * cmspk::term::RawTerminalApp
// * cmspk::term::VtInputSource
// * cmspk::io::BasicDataSourceFromFile
//
// ---
// User Manual
// ---
// ## General behaviour
//
// * Pressing a key will usually print whether it is a special key, if it recognizes it ;
// * otherwise it prints the sequence of key codes it has received.
// * printable characters in the US-ASCII range are also displayed as character
//
// ## Special keys
//
// * 'q' (lower case, case sensitive) will quit
// * ctrl+'l' will emit a cursor location query to receive a cursor position report that will be printed (row and column).
// ---

// standard includes
#include <unistd.h>

#include <iostream>
#include <string>

// other includes
#include "cmspk/term/RawTerminalApp.hpp"
#include "cmspk/term/VtInputSource.hpp"
#include "cmspk/io/DataSourceFromFile.hpp"

using AsciiCharSourceFromFile = cmspk::io::BasicDataSourceFromFile<char8_t, char8_t>;

// By virtue of the RAII pattern and static storage,
// the terminal raw mode will be enabled before entering `main()`
static cmspk::term::RawTerminalApp app({.clearScreenPolicyOnStartup = cmspk::term::ClearScreenPolicy::DO_NOTHING,
                                        .clearScreenPolicyOnShutdown = cmspk::term::ClearScreenPolicy::CLEAR_SCREEN});


// BEGIN To be moved -- VtOut

// TODO -- complete VtOut : setCursorPosition, setColors(std::optional<VtColor666> front, std::optional<VtColor666> back),...

struct VtOutReportSingle { // TODO: from DataSinkReport
	size_t countWritten; //how much data was written
	std::optional<cmspk::io::IoErrorAscii> error; //if present, an error interrupted.
};
/*
struct VtOutReportBatch { // TODO: from DataSinkReport
	size_t countItemWritten; //how much item of the list was written
	VtOutReportSingle lastItemReport;
};
*/
class VtOut { // TODO: implements DataSink<T, CharT>
  public:
    VtOutReportSingle push(const std::basic_string<char8_t>& data) {
		// OVERRIDE DataSink<T, CharT>.push(const T& data)
		size_t report = write(STDOUT_FILENO, data.c_str(), data.size());
		// TODO : May report be <0 for error ?
        if (report < data.size()) {
            return {.countWritten = report, .error = {/*TODO*/}};
        } else {
            return {.countWritten = report, .error = std::nullopt};
        }
	};
    VtOutReportSingle queryCursorPosition() { return push(u8"\x1b[6n"); };

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

// TODO static VtOut vtOut
static VtOut vtOut ;

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
    if (vtin == cmspk::term::VtInputKey::ctrl_l) {
		std::cout << "Query cursor position...";
		vtOut.queryCursorPosition();
		std::cout << cmspk::term::ASCII_RAW_ENDL;
	}
}

void onReportInput(cmspk::term::VtInputReport vtin) {
	if (!std::holds_alternative<cmspk::term::VtInputCursorPositionReport>(vtin)) return; // unsupported, ignore
	cmspk::term::VtInputCursorPositionReport report = std::get<cmspk::term::VtInputCursorPositionReport>(vtin);
	std::cout << "Cursor position report: \tRow = " << report.row << "\tCol = " << report.col << cmspk::term::ASCII_RAW_ENDL;
}

int main() {
    cmspk::term::VtInputSource source(std::unique_ptr<cmspk::io::BasicDataSource<char8_t, char8_t>>(new AsciiCharSourceFromFile(STDIN_FILENO)));
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
            } else if (std::holds_alternative<cmspk::term::VtInputReport>(in)) {
				onReportInput(std::get<cmspk::term::VtInputReport>(in));
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
