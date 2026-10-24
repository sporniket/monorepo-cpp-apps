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
    VtOutReportSingle clearScreen() {return push(u8"\x1b[2J");}
    VtOutReportSingle putCursorHome() {return push(u8"\x1b[H");}
    VtOutReportSingle putCursorNextLine() {return push(u8"\n\r");}

};

// END To be moved

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
template<class CharT>
struct BasicTextShard {

    /**
     * The actual content of the shard.
     */
    std::basic_string<CharT> raw ;

    /**
     * If present, the text to display, if absent, display the raw text directly.
     */
    std::optional<std::basic_string<CharT>> rendered = std::nullopt;
};

/**
 * The editor's text buffer is a collection of text shards.
 */
template<class CharT>
using BasicTextBuffer = std::vector<BasicTextShard<CharT>>;

/**
 * A line of text is a list of text shards from the text buffer.
 */
using TextLineBody = std::vector<size_t>;

template<class CharT>
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
        TextLineBody& line = lines.back(); //ASSUME cursor at the last line
        size_t index = line.back(); // ASSUME cursor at the last shard of the line
        BasicTextShard<CharT>& shard = buffer.at(index) ;
        // ASSUME cursor at the end of the shard
        shard.raw.push_back(c);
    }
};

// ------------
// RUNTIME
// ------------
using Document = BasicTextDocument<char8_t>;
static Document theDocument ;
// =====================================================================

static bool isRunning = true;  // crude state machine
static bool needRedraw = false ; // crude state machine

// TODO static VtOut vtOut
static VtOut vtOut ;

void notifyQuit() { isRunning = false; }

void notifyRender() {needRedraw = true; }

void notifyDoneRender() {needRedraw = false; }

void onCharVtInput(char8_t vtin) {
    if (vtin >= 32 && vtin < 128) {
        theDocument.put(vtin) ;
        notifyRender();
    }
}

void onKeyVtInput(cmspk::term::VtInputKey vtin) {
    if (vtin == cmspk::term::VtInputKey::ctrl_q) {
        notifyQuit();
	}
}

void onReportInput(cmspk::term::VtInputReport vtin) {
	if (!std::holds_alternative<cmspk::term::VtInputCursorPositionReport>(vtin)) return; // unsupported, ignore
	cmspk::term::VtInputCursorPositionReport report = std::get<cmspk::term::VtInputCursorPositionReport>(vtin);
	std::cout << "Cursor position report: \tRow = " << report.row << "\tCol = " << report.col << cmspk::term::ASCII_RAW_ENDL;
}

void render() {
    // TODO
    vtOut.putCursorHome();

    // ---
    // render edit area
    // ---
    bool isFirstLine = true;
    for(TextLineBody line:theDocument.lines) {
        if(!isFirstLine) {
            vtOut.putCursorNextLine();
        } else {
            isFirstLine = false;
        }
        for(size_t index:line) {
            if (index >= theDocument.buffer.size()) continue;
            BasicTextShard<char8_t> shard = theDocument.buffer.at(index);
            if(shard.rendered) {
                vtOut.push(*(shard.rendered));
            } else {
                vtOut.push(shard.raw);
            }
        }
    }

    // ---
    // render "after text" area
    // ---
    for(uint8_t i = 0 ; i < 10 ; i++) {
        vtOut.putCursorNextLine();
        vtOut.push(u8"~");
    }

    // ---
    // render document status
    // ---
    vtOut.putCursorNextLine();
    vtOut.push(u8"===(unsaved note 01)===0 chars============(col,row):001,001===");

    // ---
    // render application panel
    // ---
    vtOut.putCursorNextLine();
    vtOut.push(u8"---EDIT MODE---                                    (ESC for command mode)");
    vtOut.putCursorNextLine();
    vtOut.push(u8"   Ct-Q : quit   --    Ct-S : save    --    Ct-O : load");
    vtOut.putCursorNextLine();
    vtOut.push(u8"   Ct-X : cut    --    Ct-C : copy    --    Ct-V : paste");
    vtOut.putCursorNextLine();
    vtOut.push(u8"   Ct-Z : undo   -- Ct-Sh-Z : redo    -- Ct-Sh-R : reload");
    vtOut.putCursorNextLine();
    vtOut.push(u8"   Ct-F : find   --    Ct-H : replace --      F1 : help");

    // TODO put cursor at edition point
}

void init() {
    theDocument.init(); // FIXME change code so that it is not mandatory (otherwise, theDocument.put() fails catastrophically
    vtOut.clearScreen();
    render() ;
}

int main() {
    init() ;
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
        if(needRedraw) {
            render();
            notifyDoneRender() ;
        }

        // REPL -- Loop
    }
    return 0;
}
// after main, app is destroyed, which restores terminal 'cooked' mode and
// clears the screen before printing the error message given with `app.die(...)`.
