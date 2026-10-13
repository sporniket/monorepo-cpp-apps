// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 David SPORN
// ---
// This is part of **term library by sporniket**.
// A collection of utilities for writing terminal-hosted applications.
// ---

// standard libs
#include <cstdint>
#include <expected>
#include <iostream>
#include <memory>
#include <optional>
#include <variant>
#include <vector>

// testing framework
#include <criterion/criterion.h>

// project
#include <string>

#include "cmspk/io/DataSourceMock.hpp"

// ================[ BEGIN common code ]==================

// ================[ END common code ]==================

// ================[ BEGIN test suite ]==================
// Parameterized tests are a hassle, see it later
Test(DataSourceMock, can_be_created_from_a_sequence_containing_only_values) {
    std::vector<char8_t> toBeTested{0, 28, 29, 30, 31};
    std::unique_ptr<cmspk::io::BasicDataSource<char8_t, char8_t>> source(new cmspk::io::DataSourceMock<char8_t, char8_t>(toBeTested));
    for (char8_t c : toBeTested) {
        std::expected<char8_t, cmspk::io::IoErrorAscii> nextChar = source->next();
        if (nextChar) {
            cr_assert((*nextChar) == c, "expected nextChar to be %d, got %d", (uint16_t)c, (uint16_t)(*nextChar));
        } else {
            cr_assert_fail("should not reach here, expected %d, got error message %s", (uint16_t)c, (const char*)(nextChar.error().message.c_str()));
        }
    }
    std::expected<char8_t, cmspk::io::IoErrorAscii> nextChar = source->next();
    cr_assert(not(nextChar), "should return an error");
    cr_assert(cmspk::io::IoErrorType::END_OF_DATA == nextChar.error().type);
}

Test(DataSourceMock, can_be_created_from_a_sequence_containing_a_mix_of_values_and_errors) {
    std::vector<std::variant<char8_t, cmspk::io::BasicIoError<char8_t>>> toBeTested{
        (char8_t)0,  (char8_t)28,
        (char8_t)29, cmspk::io::BasicIoError<char8_t>({.type = cmspk::io::IoErrorType::NOT_READY, .message = u8"not.ready", .details = {}}),
        (char8_t)30, (char8_t)31};
    std::unique_ptr<cmspk::io::BasicDataSource<char8_t, char8_t>> source(new cmspk::io::DataSourceMock<char8_t, char8_t>(toBeTested));
    uint16_t count = 0;
    for (std::variant<char8_t, cmspk::io::BasicIoError<char8_t>> c : toBeTested) {
        count++;
        std::expected<char8_t, cmspk::io::BasicIoError<char8_t>> nextChar = source->next();
        if (std::holds_alternative<char8_t>(c)) {
            cr_assert(nextChar, "expected nextChar to be expected at pos %d", count);
            cr_assert((*nextChar) == std::get<char8_t>(c), "expected nextChar to be %d, got %d", (uint16_t)std::get<char8_t>(c), (uint16_t)(*nextChar));
        } else {
            cr_assert(not(nextChar), "expected nextChar to be unexpected at pos %d", count);
            cr_assert(nextChar.error().type == std::get<cmspk::io::BasicIoError<char8_t>>(c).type, "expected nextChar.error().type to be %d, got %d",
                      (uint16_t)std::get<cmspk::io::BasicIoError<char8_t>>(c).type, nextChar.error().type);
        }
    }
    std::expected<char8_t, cmspk::io::IoErrorAscii> nextChar = source->next();
    cr_assert(not(nextChar), "should return an error");
    cr_assert(cmspk::io::IoErrorType::END_OF_DATA == nextChar.error().type);
}

// ================[ END test suite ]==================
