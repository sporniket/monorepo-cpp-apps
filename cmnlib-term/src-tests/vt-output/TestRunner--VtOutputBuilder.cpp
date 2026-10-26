// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 David SPORN
// ---
// This is part of **term library by sporniket**.
// A collection of utilities for writing terminal-hosted applications.
// ---
// @since v0.0.2

// standard libs
#include <cstdint>
#include <memory>

// testing framework
#include <criterion/criterion.h>

// project
#include <string>

#include "cmspk/term/VtOutputBuilder.hpp"

// ================[ BEGIN common code ]==================

// ================[ END common code ]==================

// ================[ BEGIN test suite ]==================
// Parameterized tests are a hassle, see it later
Test(BasicVtOutputBuilder, should_be_usable_fluently) {
    /// __when__ using a builder in a fluent way, __then__ it create the output.
    cr_assert(*(cmspk::term::BasicVtOutputBuilder<char8_t>::create()->append(u8"created on the fly, ")->append(u8"the builder won't be reused")->build()) ==
              u8"created on the fly, the builder won't be reused");
}

Test(BasicVtOutputBuilder, should_be_usable_multiple_times) {
    /// __given__ a new builder
    std::unique_ptr<cmspk::term::BasicVtOutputBuilder<char8_t>> builder = cmspk::term::BasicVtOutputBuilder<char8_t>::create();

    /// __when__ an output has already been built
    cr_assert(*(builder->append(u8"first sequence")->append(u8" built on never used builder")->build()) == u8"first sequence built on never used builder");

    /// __then__ it is possible to reuse the builder to build another output.
    cr_assert(*(builder->append(u8"Another sequence")->build()) == u8"Another sequence");
}

Test(BasicVtOutputBuilder__append, should_append_given_text_to_the_result) {
    /// __given__ a new builder
    std::unique_ptr<cmspk::term::BasicVtOutputBuilder<char8_t>> builder = cmspk::term::BasicVtOutputBuilder<char8_t>::create();

    /// __when__ calling `append()` with various texts
    builder->append(u8"hello ")->append(u8"world ")->append(u8"!");

    /// __then__ the result is the concatenation of all those texts
    std::unique_ptr<std::basic_string<char8_t>> result = builder->build();
    cr_assert((*result) == u8"hello world !");
}

Test(BasicVtOutputBuilder__queryCursorPosition, should_append_command_n_6_to_the_result) {
    /// __given__ a new builder
    std::unique_ptr<cmspk::term::BasicVtOutputBuilder<char8_t>> builder = cmspk::term::BasicVtOutputBuilder<char8_t>::create();

    /// __when__ calling `queryCursorPosition()` with various texts
    builder->queryCursorPosition();

    /// __then__ it appends the sequence `ESC + '[6n'`
    std::unique_ptr<std::basic_string<char8_t>> result = builder->build();
    cr_assert(result->size() == 4);
    cr_assert(result->at(0) == 27, "expected 27, got %d", (uint16_t)result->at(0));
    cr_assert((*result) == u8"\x1b[6n");
}

Test(BasicVtOutputBuilder__clearScreen, should_append_command_J_2_to_the_result) {
    /// __given__ a new builder
    std::unique_ptr<cmspk::term::BasicVtOutputBuilder<char8_t>> builder = cmspk::term::BasicVtOutputBuilder<char8_t>::create();

    /// __when__ calling `clearScreen()` with various texts
    builder->clearScreen();

    /// __then__ it appends the sequence `ESC + '[2J'`
    std::unique_ptr<std::basic_string<char8_t>> result = builder->build();
    cr_assert((*result) == u8"\x1b[2J");
}

Test(BasicVtOutputBuilder__goHome, should_append_command_H_to_the_result) {
    /// __given__ a new builder
    std::unique_ptr<cmspk::term::BasicVtOutputBuilder<char8_t>> builder = cmspk::term::BasicVtOutputBuilder<char8_t>::create();

    /// __when__ calling `goHome()` with various texts
    builder->goHome();

    /// __then__ it appends the sequence `ESC + '[H'`
    std::unique_ptr<std::basic_string<char8_t>> result = builder->build();
    cr_assert((*result) == u8"\x1b[H");
}

Test(BasicVtOutputBuilder__goNextLine, should_append_crlf_sequence_to_the_result) {
    /// __given__ a new builder
    std::unique_ptr<cmspk::term::BasicVtOutputBuilder<char8_t>> builder = cmspk::term::BasicVtOutputBuilder<char8_t>::create();

    /// __when__ calling `goNextLine()`
    builder->goNextLine();

    /// __then__ it insert the sequence `CR + LF`
    std::unique_ptr<std::basic_string<char8_t>> result = builder->build();
    cr_assert((*result) == u8"\r\n");
}

// ================[ END test suite ]==================
