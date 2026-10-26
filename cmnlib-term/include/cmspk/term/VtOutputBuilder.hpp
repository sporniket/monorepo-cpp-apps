// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 David SPORN
// ---
// This is part of **term library by sporniket**.
// A collection of utilities for writing terminal-hosted applications.
// ---

#ifndef __CMSPK__TERM__VTOUTPUTBUILDER_HPP__
#define __CMSPK__TERM__VTOUTPUTBUILDER_HPP__

// standard libs
#include <cstdint>
#include <memory>
#include <string>

// project
#include "cmspk/io/IoError.hpp"  // -- required for cmspk::io::convertUsAsciiAndControlString

namespace cmspk::term {
// ================[ CODE BEGINS ]================

/**
 * Helper to build a string containing virtual terminal escape sequences.
 *
 * It can be used in two ways : either a single throw-away use, or kept for reuse.
 *
 * ## Requirements
 *
 * *[Specfication guidelines](../../README--specification-guidelines.md)
 *
 * @since v0.0.3
 */
template <class CharT>
class BasicVtOutputBuilder {
  public:
    static std::unique_ptr<BasicVtOutputBuilder> create() noexcept { return std::make_unique<BasicVtOutputBuilder<CharT>>(); }
    BasicVtOutputBuilder<CharT>* append(const std::basic_string<CharT>& text) noexcept {
        if (!buffer) recycle();
        buffer->append(text);
        return this;
    };

    BasicVtOutputBuilder<CharT>* queryCursorPosition() noexcept {
        if (!buffer) recycle();
        buffer->append(sequence_query_cursor_position);
        return this;
    };
    BasicVtOutputBuilder<CharT>* clearScreen() noexcept {
        if (!buffer) recycle();
        buffer->append(sequence_clear_screen);
        return this;
    }
    BasicVtOutputBuilder<CharT>* goHome() noexcept {
        if (!buffer) recycle();
        buffer->append(sequence_go_home);
        return this;
    }
    BasicVtOutputBuilder<CharT>* goNextLine() noexcept {
        if (!buffer) recycle();
        buffer->append(sequence_go_next_line);
        return this;
    }
    std::unique_ptr<std::basic_string<CharT>> build() noexcept {
        if (!buffer) recycle();
        return std::move(buffer);
    }

  private:
    std::unique_ptr<std::basic_string<CharT>> buffer = std::make_unique<std::basic_string<CharT>>();
    static constexpr std::basic_string<CharT> sequence_query_cursor_position = cmspk::io::convertUsAsciiAndControlString<CharT>(u8"\x1b[6n");
    static constexpr std::basic_string<CharT> sequence_clear_screen = cmspk::io::convertUsAsciiAndControlString<CharT>(u8"\x1b[2J");
    static constexpr std::basic_string<CharT> sequence_go_home = cmspk::io::convertUsAsciiAndControlString<CharT>(u8"\x1b[H");
    static constexpr std::basic_string<CharT> sequence_go_next_line = cmspk::io::convertUsAsciiAndControlString<CharT>(u8"\r\n");

    void recycle() { buffer = std::make_unique<std::basic_string<CharT>>(); }
};

// ================[ END OF CODE ]================
}  // namespace cmspk::term
#endif
