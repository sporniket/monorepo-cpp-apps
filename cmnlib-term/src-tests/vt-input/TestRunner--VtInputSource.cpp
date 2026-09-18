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
            return std::unexpected(cmspk::io::IoErrorAscii{.type = cmspk::io::IoErrorType::END_OF_DATA, .message = u8"end.of.file", .details = {}});
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
    /// > This is the fall-back behaviour. The list [28,29,30,31] is what remains unknown when all other behaviours have been implemented.
    /// __given__ a character data source that will return the sequence : `[28,29,30,31]`.
    std::vector<char8_t> toBeTested{28, 29, 30, 31};

    /// __given__ the VtInputSource under test is plugged to that character data source.
    cmspk::term::VtInputSource source(std::unique_ptr<cmspk::io::BasicDataSource<char8_t, char8_t>>(new DataSourceFromSequence(toBeTested)));

    for (char8_t c : toBeTested) {
        /// __when__ reading the next VtInput with `next()` as many time as the length of the sequence.
        std::expected<cmspk::term::VtInput, cmspk::io::IoErrorAscii> nextInput = source.next();

        /// __then__ each time the VtInputSource will return a `std::variant` containing a `VtInputUnknown` having the expected `rawValue`.
        if (nextInput) {
            cr_assert(std::holds_alternative<cmspk::term::VtInputUnknown>(*nextInput));
            cmspk::term::VtInputUnknown vtin = std::get<cmspk::term::VtInputUnknown>(*nextInput);
            cr_assert(vtin.rawValue == c, "expected vtin.rawValue == %d, got %d", (uint16_t)c, (uint16_t)vtin.rawValue);
        } else {
            cr_assert_fail("should not reach here, data source got %d, got error message %s", (uint16_t)c, (const char*)(nextInput.error().message.c_str()));
        }
    }

    /// __then__ reading the next VtInput returns and end of file error.
    std::expected<cmspk::term::VtInput, cmspk::io::IoErrorAscii> nextChar = source.next();
    cr_assert(not(nextChar), "should return an error");
    cr_assert(cmspk::io::IoErrorType::END_OF_DATA == nextChar.error().type);
}

Test(VtInputSource, should_process_single_octet_values__printable_characters_and_keys) {
    /// __given__ a character data source that will return a mixed list of single-octet printable characters and single-octet keys (except ESCAPE).
    std::vector<char8_t> toBeTested{0, 1, 2, 3, 4, 5, 26, 32, 65, 66, 13};  // a sample of known inputs

    /// __given__ the VtInputSource under test is plugged to that character data source.
    cmspk::term::VtInputSource source(std::unique_ptr<cmspk::io::BasicDataSource<char8_t, char8_t>>(new DataSourceFromSequence(toBeTested)));

    uint16_t count = 0;
    std::vector<cmspk::term::VtInput> toBeExpected{cmspk::term::VtInputKey::CTRL_SPACE,
                                                   cmspk::term::VtInputKey::CTRL_A,
                                                   cmspk::term::VtInputKey::CTRL_B,
                                                   cmspk::term::VtInputKey::CTRL_C,
                                                   cmspk::term::VtInputKey::CTRL_D,
                                                   cmspk::term::VtInputKey::CTRL_E,
                                                   cmspk::term::VtInputKey::CTRL_Z,
                                                   (char8_t)' ',
                                                   (char8_t)'A',
                                                   (char8_t)'B',
                                                   cmspk::term::VtInputKey::RETURN};
    for (cmspk::term::VtInput expectedInput : toBeExpected) {
        /// __when__ reading the next VtInput with `next()` as many time as the length of the sequence.
        std::expected<cmspk::term::VtInput, cmspk::io::IoErrorAscii> nextInput = source.next();

        /// __then__ each time the VtInputSource will return a `std::variant` containing the expected `char8_t` or `VtInputKey`.
        if (nextInput) {
            if (std::holds_alternative<cmspk::term::VtInputKey>(expectedInput)) {
                cr_assert(std::holds_alternative<cmspk::term::VtInputKey>(*nextInput), "expected VtInputKey at index #%d", count);
                cr_assert(std::get<cmspk::term::VtInputKey>(*nextInput) == std::get<cmspk::term::VtInputKey>(expectedInput),
                          "did not get expected VtInputKey at index #%d", count);
            } else if (std::holds_alternative<char8_t>(expectedInput)) {
                cr_assert(std::holds_alternative<char8_t>(*nextInput), "expected char8_t at index #%d", count);
                cr_assert(std::get<char8_t>(*nextInput) == std::get<char8_t>(expectedInput), "did not get expected char8_t at index #%d", count);
            }
        } else {
            cr_assert_fail("should not reach here at loop #%d, got error message %s", count, (const char*)(nextInput.error().message.c_str()));
        }
        count++;
    }

    /// __then__ reading the next VtInput returns and end of file error.
    std::expected<cmspk::term::VtInput, cmspk::io::IoErrorAscii> nextChar = source.next();
    cr_assert(not(nextChar), "should return an error");
    cr_assert(cmspk::io::IoErrorType::END_OF_DATA == nextChar.error().type);
}
// ================[ END test suite ]==================
