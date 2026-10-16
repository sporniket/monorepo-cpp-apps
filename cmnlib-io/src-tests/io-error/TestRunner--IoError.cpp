// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 David SPORN
// ---
// This is part of **term library by sporniket**.
// A collection of utilities for writing terminal-hosted applications.
// ---

// standard libs
#include <cstdint>
#include <string>

// testing framework
#include <criterion/criterion.h>

// project
#include "cmspk/io/IoError.hpp"

// ================[ BEGIN common code ]==================

// ================[ END common code ]==================

// ================[ BEGIN test suite ]==================
// Parameterized tests are a hassle, see it later


Test(convertUsAsciiString, should_convert_only_us_ascii_characters) {
    /// __given__ an input string containing characters that are not US-ASCII
    std::basic_string<char8_t> in(u8"évènement héroïque");

    /// __when__ calling convertUsAsciiString()
    std::basic_string<char32_t> converted = cmspk::io::convertUsAsciiString<char32_t>(in) ;

    /// __then__ the result converted those characters to one or more '?'
    cr_assert(converted.compare(U"??v??nement h??ro??que") == 0);

}

// ================[ END test suite ]==================
