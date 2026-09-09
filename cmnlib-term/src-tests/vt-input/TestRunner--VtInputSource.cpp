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

// TO BE MOVED

#include "cmspk/term/VtInputSource.hpp"

// ================[ BEGIN common code ]==================
class DataSourceFromSequence : public cmspk::io::BasicDataSource<char8_t, char8_t> {
  public:
    DataSourceFromSequence(std::vector<char8_t> sequence) : sequence(sequence) { cursor = this->sequence.begin(); }
    virtual std::expected<char8_t, cmspk::io::IoErrorAscii> next() {
        if (!hasNext()) {
            return std::unexpected(
                cmspk::io::IoErrorAscii{.type = cmspk::io::IoErrorType::END_OF_DATA, .message = (const char8_t*)"end.of.file", .details = {}});
        }
        char8_t result = *cursor;
        ++cursor;
        return result;
    }
    virtual bool hasNext() { return cursor != sequence.end(); }

    virtual ~DataSourceFromSequence() {}
    /**
     * Copy operation (rule of 5).
     */
    DataSourceFromSequence(const DataSourceFromSequence&) = default;

    /**
     * Copy operator (rule of 5).
     */
    DataSourceFromSequence& operator=(const DataSourceFromSequence&) = default;

    /**
     * Move operation (rule of 5).
     */
    DataSourceFromSequence(DataSourceFromSequence&&) = default;

    /**
     * Move operator (rule of 5).
     */
    DataSourceFromSequence& operator=(DataSourceFromSequence&&) = default;

  private:
    std::vector<char8_t> sequence;
    std::vector<char8_t>::iterator cursor;
};

// TO BE MOVED
// ================[ END common code ]==================

// ================[ BEGIN test suite ]==================
// Parameterized tests are a hassle, see it later
Test(DataSourceFromSequence, should_return_each_value_of_source_vector) {
    std::vector<char8_t> toBeTested{0, 28, 29, 30, 31};
    std::unique_ptr<cmspk::io::BasicDataSource<char8_t, char8_t>> source(new DataSourceFromSequence(toBeTested));
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

Test(VtInputSource, should_process_single_octet_values__unknown_values) {
    std::vector<char8_t> toBeTested{0, 28, 29, 30, 31};
    cmspk::term::VtInputSource source(std::unique_ptr<cmspk::io::BasicDataSource<char8_t, char8_t>>(new DataSourceFromSequence(toBeTested)));
    for (char8_t c : toBeTested) {
        std::expected<cmspk::term::VtInput, cmspk::io::IoErrorAscii> nextInput = source.next();
        if (nextInput) {
            cr_assert(std::holds_alternative<cmspk::term::VtInputUnknown>(*nextInput));
            cmspk::term::VtInputUnknown vtin = std::get<cmspk::term::VtInputUnknown>(*nextInput);
            cr_assert(vtin.rawValue == c, "expected vtin.rawValue == %d, got %d", (uint16_t)c, (uint16_t)vtin.rawValue);
        } else {
            cr_assert_fail("should not reach here, data source got %d, got error message %s", (uint16_t)c, (const char*)(nextInput.error().message.c_str()));
        }
    }
    std::expected<cmspk::term::VtInput, cmspk::io::IoErrorAscii> nextChar = source.next();
    cr_assert(not(nextChar), "should return an error");
    cr_assert(cmspk::io::IoErrorType::END_OF_DATA == nextChar.error().type);
}

// ================[ END test suite ]==================
