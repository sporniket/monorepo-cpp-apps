// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 David SPORN
// ---
// sped -- SPorniket's EDitor
// ---
// A _TUI_ (Textual User Interface) text editor, inspired by user experience that
// was common on MS-DOS applications, and a little bit of modern user experience.
// ---
// User Manual
// ---
// ## The two key bindings to know
// * CTRL+Q : Will always ask to confirm, and if yes, quit the program
// * ESC : goes back to menu mode
// ---
// @since v0.0.3

// standard includes
#include <unistd.h>

#include <iostream>
#include <string>

// other includes
#include "cmspk/io/DataSourceFromFile.hpp"
#include "cmspk/io/StringSinkToFile.hpp"
#include "cmspk/term/RawTerminalApp.hpp"
#include "cmspk/term/VtInputSource.hpp"
#include "cmspk/term/VtOutputBuilder.hpp"

using AsciiCharSourceFromFile = cmspk::io::BasicDataSourceFromFile<char8_t, char8_t>;

// By virtue of the RAII pattern and static storage,
// the terminal raw mode will be enabled before entering `main()`
static cmspk::term::RawTerminalApp app({.clearScreenPolicyOnStartup = cmspk::term::ClearScreenPolicy::DO_NOTHING,
                                        .clearScreenPolicyOnShutdown = cmspk::term::ClearScreenPolicy::CLEAR_SCREEN});

static std::unique_ptr<cmspk::term::BasicVtOutputBuilder<char8_t>> vtOutBuilder = cmspk::term::BasicVtOutputBuilder<char8_t>::create();
static cmspk::io::BasicStringSinkToFile<char8_t> vtOut(STDOUT_FILENO);

// =====================================================================
// Editor data struct
enum class TextShardType {
    normal,
    htabs,
    keyword1,
    keyword2,
    keyword3,
    comment,
    // and so on.
};
/**
 * The basic building bloc of the model for text editing.
 *
 * A shard of text is an homogenous sequence of characters. Homogenous means
 * that all the same characters will have the same properties, like :
 *
 * <ol>
 * <li>color rendering</li>
 * <li>number of rendered characters per raw characters</li>
 * </ol>
 */
template <class CharT>
struct BasicTextShard {
    /**
     * The actual content of the shard.
     */
    std::basic_string<CharT> raw;

    /**
     * If present, the text to display, if absent, display the raw text directly.
     */
    std::optional<std::basic_string<CharT>> rendered = std::nullopt;
};

/**
 * The editor's text buffer is a collection of text shards.
 */
template <class CharT>
using BasicTextBuffer = std::vector<BasicTextShard<CharT>>;

/**
 * A line of text is a list of text shards from the text buffer.
 */
using TextLineBody = std::vector<size_t>;

template <class CharT>
struct BasicTextDocument {
    BasicTextBuffer<CharT> buffer;
    std::vector<TextLineBody> lines;

    void init() {
        buffer.reserve(1024);
        lines.reserve(512);
        buffer.push_back(BasicTextShard<CharT>{});
        lines.push_back({0});
    }

    void put(CharT c) {
        // FIXME protect against empty document, return std::expected<void, TextDocumentError>
        TextLineBody& line = lines.back();  // ASSUME cursor at the last line
        size_t index = line.back();         // ASSUME cursor at the last shard of the line
        BasicTextShard<CharT>& shard = buffer.at(index);
        // ASSUME cursor at the end of the shard
        shard.raw.push_back(c);
    }
};

// ------------
// RUNTIME
// ------------
using Document = BasicTextDocument<char8_t>;
static Document theDocument;
// =====================================================================

static bool isRunning = true;    // crude state machine
static bool needRedraw = false;  // crude state machine

void notifyQuit() { isRunning = false; }

void notifyRender() { needRedraw = true; }

void notifyDoneRender() { needRedraw = false; }

void onCharVtInput(char8_t vtin) {
    if (vtin >= 32 && vtin < 128) {
        theDocument.put(vtin);
        notifyRender();
    }
}

void onKeyVtInput(cmspk::term::VtInputKey vtin) {
    if (vtin == cmspk::term::VtInputKey::ctrl_q) {
        notifyQuit();
    }
}

void onReportInput(cmspk::term::VtInputReport vtin) {
    if (!std::holds_alternative<cmspk::term::VtInputCursorPositionReport>(vtin)) return;  // unsupported, ignore
    cmspk::term::VtInputCursorPositionReport report = std::get<cmspk::term::VtInputCursorPositionReport>(vtin);
    std::cout << "Cursor position report: \tRow = " << report.row << "\tCol = " << report.col << cmspk::term::ASCII_RAW_ENDL;
}

void render() {
    // TODO
    vtOut.push(*(vtOutBuilder->goHome()->build()));

    // ---
    // render edit area
    // ---
    bool isFirstLine = true;
    for (TextLineBody line : theDocument.lines) {
        if (!isFirstLine) {
            vtOutBuilder->goNextLine();
        } else {
            isFirstLine = false;
        }
        for (size_t index : line) {
            if (index >= theDocument.buffer.size()) continue;
            BasicTextShard<char8_t> shard = theDocument.buffer.at(index);
            if (shard.rendered) {
                vtOutBuilder->append(*(shard.rendered));
            } else {
                vtOutBuilder->append(shard.raw);
            }
        }
    }
    vtOut.push(*(vtOutBuilder->build()));

    // ---
    // render "after text" area
    // ---
    for (uint8_t i = 0; i < 10; i++) {
        vtOutBuilder->goNextLine()->append(u8"~");
    }
    vtOut.push(*(vtOutBuilder->build()));

    // ---
    // render document status
    // ---
    vtOutBuilder->goNextLine()->append(u8"===(unsaved note 01)===0 chars=======================(col,row):001,001===");
    vtOut.push(*(vtOutBuilder->build()));

    // ---
    // render application panel
    // ---
    vtOutBuilder->goNextLine()
        ->append(u8"---EDIT MODE---                                    (ESC for command mode)")
        ->goNextLine()
        ->append(u8"   Ct-Q : quit   --    Ct-S : save    --    Ct-O : load")
        ->goNextLine()
        ->append(u8"   Ct-X : cut    --    Ct-C : copy    --    Ct-V : paste")
        ->goNextLine()
        ->append(u8"   Ct-Z : undo   -- Ct-Sh-Z : redo    -- Ct-Sh-R : reload")
        ->goNextLine()
        ->append(u8"   Ct-F : find   --    Ct-H : replace --      F1 : help");
    vtOut.push(*(vtOutBuilder->build()));

    // TODO put cursor at edition point
}

void init() {
    theDocument.init();  // FIXME change code so that it is not mandatory (otherwise, theDocument.put() fails catastrophically
    vtOut.push(*(vtOutBuilder->clearScreen()->build()));
    render();
}

int main() {
    init();
    // REPL loop
    cmspk::term::VtInputSource source(std::unique_ptr<cmspk::io::BasicDataSource<char8_t, char8_t>>(new AsciiCharSourceFromFile(STDIN_FILENO)));
    while (isRunning) {
        // REPL -- Read
        std::expected<cmspk::term::VtInput, cmspk::io::IoErrorAscii> nextInput = source.next();

        // REPL -- Execute
        if (nextInput) {
            cmspk::term::VtInput in = (*nextInput);
            if (std::holds_alternative<char8_t>(in)) {
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

        // REPL -- Print
        if (needRedraw) {
            render();
            notifyDoneRender();
        }

        // REPL -- Loop
    }
    return 0;
}
// after main, app is destroyed, which restores terminal 'cooked' mode and
// clears the screen before printing the error message given with `app.die(...)`.
