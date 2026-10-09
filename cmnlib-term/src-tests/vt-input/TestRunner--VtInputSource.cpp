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
template <class T, class CharT>
class DataSourceMock : public cmspk::io::BasicDataSource<T, CharT> {
  public:
    DataSourceMock(std::vector<T> values) {
        sequence.reserve(values.size());
        for (T v : values) {
            sequence.push_back(v);
        }
        cursor = this->sequence.begin();
    }
    DataSourceMock(std::vector<std::variant<T, cmspk::io::BasicIoError<CharT>>> sequence) : sequence(sequence) { cursor = this->sequence.begin(); }
    virtual std::expected<T, cmspk::io::BasicIoError<CharT>> next() {
        if (!hasNext()) {
            return std::unexpected(cmspk::io::BasicIoError<CharT>{.type = cmspk::io::IoErrorType::END_OF_DATA, .message = {}, .details = {}});
        }
        std::variant<T, cmspk::io::BasicIoError<CharT>> result = *cursor;
        ++cursor;
        if (std::holds_alternative<cmspk::io::BasicIoError<CharT>>(result)) {
            return std::unexpected(std::get<cmspk::io::BasicIoError<CharT>>(result));
        }
        return std::get<T>(result);
    }
    virtual bool hasNext() { return cursor != sequence.end(); }

    virtual ~DataSourceMock() {}
    /**
     * Copy operation (rule of 5).
     */
    DataSourceMock(const DataSourceMock&) = default;

    /**
     * Copy operator (rule of 5).
     */
    DataSourceMock& operator=(const DataSourceMock&) = default;

    /**
     * Move operation (rule of 5).
     */
    DataSourceMock(DataSourceMock&&) = default;

    /**
     * Move operator (rule of 5).
     */
    DataSourceMock& operator=(DataSourceMock&&) = default;

  private:
    std::vector<std::variant<T, cmspk::io::BasicIoError<CharT>>> sequence;
    std::vector<std::variant<T, cmspk::io::BasicIoError<CharT>>>::iterator cursor;
};

// TO BE MOVED
// ================[ END common code ]==================

// ================[ BEGIN test suite ]==================
// Parameterized tests are a hassle, see it later
Test(DataSourceMock, can_be_created_from_a_sequence_containing_only_values) {
    std::vector<char8_t> toBeTested{0, 28, 29, 30, 31};
    std::unique_ptr<cmspk::io::BasicDataSource<char8_t, char8_t>> source(new DataSourceMock<char8_t, char8_t>(toBeTested));
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
    std::unique_ptr<cmspk::io::BasicDataSource<char8_t, char8_t>> source(new DataSourceMock<char8_t, char8_t>(toBeTested));
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

Test(VtInputSource, should_process_single_octet_values__unknown_values) {
    /// > This is the fall-back behaviour. The list [28,29,30,31] is what remains unknown when all other behaviours have been implemented.
    /// __given__ a character data source that will return the sequence : `[28,29,30,31]`.
    std::vector<char8_t> toBeTested{28, 29, 30, 31};

    /// __given__ the VtInputSource under test is plugged to that character data source.
    cmspk::term::VtInputSource source(std::unique_ptr<cmspk::io::BasicDataSource<char8_t, char8_t>>(new DataSourceMock<char8_t, char8_t>(toBeTested)));

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
    cmspk::term::VtInputSource source(std::unique_ptr<cmspk::io::BasicDataSource<char8_t, char8_t>>(new DataSourceMock<char8_t, char8_t>(toBeTested)));

    uint16_t count = 0;
    std::vector<cmspk::term::VtInput> toBeExpected{cmspk::term::VtInputKey::ctrl_space,
                                                   cmspk::term::VtInputKey::ctrl_a,
                                                   cmspk::term::VtInputKey::ctrl_b,
                                                   cmspk::term::VtInputKey::ctrl_c,
                                                   cmspk::term::VtInputKey::ctrl_d,
                                                   cmspk::term::VtInputKey::ctrl_e,
                                                   cmspk::term::VtInputKey::ctrl_z,
                                                   (char8_t)' ',
                                                   (char8_t)'A',
                                                   (char8_t)'B',
                                                   cmspk::term::VtInputKey::return_key};
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

Test(VtInputSource, should_process_multi_octets_values__keys_and_cursor_position_report) {
    /// __given__ a character data source that will return a mixed list of multi-octets sequences, including keys and cursor position report
    // something like arrow up, arrow down, cursor position report (62;20R).
    std::vector<char8_t> toBeTested{27, 91, 65, 27, 91, 66, 27, 91, 0x36, 0x32, 59, 0x32, 0x30, 82};

    /// __given__ the VtInputSource under test is plugged to that character data source.
    cmspk::term::VtInputSource source(std::unique_ptr<cmspk::io::BasicDataSource<char8_t, char8_t>>(new DataSourceMock<char8_t, char8_t>(toBeTested)));

    uint16_t count = 0;
    std::vector<cmspk::term::VtInput> toBeExpected{cmspk::term::VtInputKey::arrow_up, cmspk::term::VtInputKey::arrow_down,
                                                   cmspk::term::VtInputCursorPositionReport({.row = 62, .col = 20})};
    for (cmspk::term::VtInput expectedInput : toBeExpected) {
        /// __when__ reading the next VtInput with `next()` as many time as the expected number of received input.
        std::expected<cmspk::term::VtInput, cmspk::io::IoErrorAscii> nextInput = source.next();

        /// __then__ each time the VtInputSource will return a `std::variant` containing the expected `VtInputKey` or `VtInputReport`.
        cr_assert(nextInput, "got error %d at position %d", (uint16_t)nextInput.error().type, count);
        if (std::holds_alternative<cmspk::term::VtInputKey>(expectedInput)) {
            cr_assert(std::holds_alternative<cmspk::term::VtInputKey>(*nextInput), "expected VtInputKey at index #%d", count);
            cr_assert(std::get<cmspk::term::VtInputKey>(*nextInput) == std::get<cmspk::term::VtInputKey>(expectedInput),
                      "did not get expected VtInputKey at index #%d", count);
        } else if (std::holds_alternative<cmspk::term::VtInputReport>(expectedInput)) {
            cr_assert(std::holds_alternative<cmspk::term::VtInputReport>(*nextInput), "expected VtInputReport at index #%d", count);
            cmspk::term::VtInputReport vtinReport = std::get<cmspk::term::VtInputReport>(*nextInput);
            if (std::holds_alternative<cmspk::term::VtInputCursorPositionReport>(std::get<cmspk::term::VtInputReport>(expectedInput))) {
                cr_assert(std::holds_alternative<cmspk::term::VtInputCursorPositionReport>(vtinReport), "expected VtInputCursorPositionReport at index #%d",
                          count);
                cmspk::term::VtInputCursorPositionReport cursorPositionReport = std::get<cmspk::term::VtInputCursorPositionReport>(vtinReport);
                cmspk::term::VtInputCursorPositionReport expectedReport =
                    std::get<cmspk::term::VtInputCursorPositionReport>(std::get<cmspk::term::VtInputReport>(expectedInput));
                cr_assert(cursorPositionReport.row == expectedReport.row, "expected cursorPositionReport.row to be %d, got %d", expectedReport.row,
                          cursorPositionReport.row);
                cr_assert(cursorPositionReport.col == expectedReport.col, "expected cursorPositionReport.row to be %d, got %d", expectedReport.col,
                          cursorPositionReport.col);
            }
        }
        count++;
    }

    /// __then__ reading the next VtInput returns and end of file error.
    std::expected<cmspk::term::VtInput, cmspk::io::IoErrorAscii> nextChar = source.next();
    cr_assert(not(nextChar), "should return an error");
    cr_assert(cmspk::io::IoErrorType::END_OF_DATA == nextChar.error().type);
}

Test(VtInputSource, should_fall_back_to_single_octet_processing_when_multi_octet_sequence_is_broken_or_unknown) {
    /// __given__ a character data source that will return a mixed list of broken and unknown multi-octets sequences
    // something like "\x1b[a\x1b[\x1[B"
    std::vector<char8_t> toBeTested{27, 91, 97, 27, 91, 27, 91, 66};

    /// __given__ the VtInputSource under test is plugged to that character data source.
    cmspk::term::VtInputSource source(std::unique_ptr<cmspk::io::BasicDataSource<char8_t, char8_t>>(new DataSourceMock<char8_t, char8_t>(toBeTested)));

    /// __when__ reading the next VtInput with `next()` as many time as the expected number of received input.
    uint16_t count = 0;
    std::vector<cmspk::term::VtInput> toBeExpected{cmspk::term::VtInputKey::escape, (char8_t)91, (char8_t)97,
                                                   cmspk::term::VtInputKey::escape, (char8_t)91, cmspk::term::VtInputKey::arrow_down};
    for (cmspk::term::VtInput expectedInput : toBeExpected) {
        std::expected<cmspk::term::VtInput, cmspk::io::IoErrorAscii> nextInput = source.next();
        /// __then__ each time the VtInputSource will return a `std::variant` containing the expected `char8_t` or `VtInputKey`.
        cr_assert(nextInput, "got error %d at index #%d", (uint16_t)nextInput.error().type, count);
        if (std::holds_alternative<cmspk::term::VtInputKey>(expectedInput)) {
            cr_assert(std::holds_alternative<cmspk::term::VtInputKey>(*nextInput), "expected VtInputKey at index #%d", count);
            cr_assert(std::get<cmspk::term::VtInputKey>(*nextInput) == std::get<cmspk::term::VtInputKey>(expectedInput),
                      "did not get expected VtInputKey at index #%d", count);
        } else if (std::holds_alternative<char8_t>(expectedInput)) {
            cr_assert(std::holds_alternative<char8_t>(*nextInput), "expected char at index #%d", count);
            char8_t c = std::get<char8_t>(*nextInput);
            cr_assert(c == std::get<char8_t>(expectedInput), "expected char %d, got %d at index #%d", (uint16_t)std::get<char8_t>(expectedInput), (uint16_t)c,
                      count);
        }
        count++;
    }

    /// __then__ reading the next VtInput returns and end of file error.
    std::expected<cmspk::term::VtInput, cmspk::io::IoErrorAscii> nextChar = source.next();
    cr_assert(not(nextChar), "should return an error");
    cr_assert(cmspk::io::IoErrorType::END_OF_DATA == nextChar.error().type);
}

Test(VtInputSource, should_fall_back_to_single_octet_processing_when_multi_octet_sequence_is_interrupted_by_lack_of_data) {
    /// __given__ a character data source that will return a starting sequence interrupted before its end
    // something like "\x1b[", then a "not ready" error, then "A\x1b[12"
    std::vector<std::variant<char8_t, cmspk::io::IoErrorAscii>> toBeTested{
        (char8_t)27,
        (char8_t)91,
        cmspk::io::BasicIoError{.type = cmspk::io::IoErrorType::NOT_READY, .message = (std::basic_string<char8_t>)u8"not.ready", .details = {}},
        (char8_t)65,
        (char8_t)27,
        (char8_t)91,
        (char8_t)0x31,
        (char8_t)0x32};

    /// __given__ the VtInputSource under test is plugged to that character data source.
    cmspk::term::VtInputSource source(std::unique_ptr<cmspk::io::BasicDataSource<char8_t, char8_t>>(new DataSourceMock<char8_t, char8_t>(toBeTested)));

    /// __when__ reading the next VtInput with `next()` as many time as the expected number of received input.
    uint16_t count = 0;
    std::vector<cmspk::term::VtInput> toBeExpected{
        cmspk::term::VtInputKey::escape, (char8_t)91, (char8_t)65, cmspk::term::VtInputKey::escape, (char8_t)91, (char8_t)0x31, (char8_t)0x32};
    for (cmspk::term::VtInput expectedInput : toBeExpected) {
        std::expected<cmspk::term::VtInput, cmspk::io::IoErrorAscii> nextInput = source.next();
        /// __then__ each time the VtInputSource will return a `std::variant` containing the expected `char8_t` or `VtInputKey`.
        cr_assert(nextInput, "got error %d at index #%d", (uint16_t)nextInput.error().type, count);
        if (std::holds_alternative<cmspk::term::VtInputKey>(expectedInput)) {
            cr_assert(std::holds_alternative<cmspk::term::VtInputKey>(*nextInput), "expected VtInputKey at index #%d", count);
            cr_assert(std::get<cmspk::term::VtInputKey>(*nextInput) == std::get<cmspk::term::VtInputKey>(expectedInput),
                      "did not get expected VtInputKey at index #%d", count);
        } else if (std::holds_alternative<char8_t>(expectedInput)) {
            cr_assert(std::holds_alternative<char8_t>(*nextInput), "expected char at index #%d", count);
            char8_t c = std::get<char8_t>(*nextInput);
            cr_assert(c == std::get<char8_t>(expectedInput), "expected char %d, got %d at index #%d", (uint16_t)std::get<char8_t>(expectedInput), (uint16_t)c,
                      count);
        }
        count++;
    }

    /// __then__ reading the next VtInput returns and end of file error.
    std::expected<cmspk::term::VtInput, cmspk::io::IoErrorAscii> nextChar = source.next();
    cr_assert(not(nextChar), "should return an error");
    cr_assert(cmspk::io::IoErrorType::END_OF_DATA == nextChar.error().type);
}
// ================[ END test suite ]==================
