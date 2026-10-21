// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 David SPORN
// ---
// This is part of **term library by sporniket**.
// A collection of utilities for writing terminal-hosted applications.
// ---
// @since v0.0.2

// standard libs
#include <cstdint>
#include <expected>
#include <string>
#include <memory>
#include <optional>
#include <variant>
#include <vector>
#include <functional>

// testing framework
#include <criterion/criterion.h>

// project
#include "cmspk/io/DataSourceFromFile.hpp"


// ================[ BEGIN common code ]==================

// ================[ END common code ]==================

// ================[ BEGIN test suite ]==================
// Parameterized tests are a hassle, see it later


// ----


struct SpyReport {
    int fileDescriptorId;
    size_t readLength;
};
Test(BasicDataSourceFromFile, should_call_the_provided_read_function) {
    /// __given__ the data source from file is given a dummy file descriptor and a spying read function
    SpyReport report{};
    auto spyRead = [&report](int fd, void* buffer, size_t length) mutable noexcept -> size_t {
        char8_t data[2]{u8"A"};
        char8_t* cBuffer = (char8_t *)buffer;
        report.fileDescriptorId = fd;
        report.readLength = length;
        cBuffer[0]=data[0];
        return 1;
    } ;
    std::unique_ptr<cmspk::io::BasicDataSource<char8_t, char8_t>> source(new cmspk::io::BasicDataSourceFromFile<char8_t, char8_t>(42, spyRead));

    /// __when__ calling next()
    std::expected<char8_t, cmspk::io::BasicIoError<char8_t>> nextChar = source->next();

    /// __then__ it has called the read function on the dummy file descriptor to get one character
    cr_assert(report.fileDescriptorId == 42);
    cr_assert(report.readLength == 1);
    cr_assert(nextChar);
    cr_assert((*nextChar) == (char8_t)65);
}

// ================[ END test suite ]==================
