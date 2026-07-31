// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 David SPORN
// ---
// This is part of **term library by sporniket**.
// A collection of utilities for writing terminal-hosted applications.
// ---

// standard libs
#include <cstdint>

// testing framework
#include <criterion/criterion.h>

// project
#include "cmspk/term/VtInput.hpp"

// ================[ BEGIN common code ]==================
class SameSingleOctetDataSource : public DataSource<char8_t> {
  public:
    virtual std::expected<T, BasicIoError<S>> next() { return value; } ;
    SameSingleOctetDataSource(char8_t value) : value(value) {};
    virtual ~SameSingleOctetDataSource() {} ;

    /**
     * Copy operation (rule of 5).
     */
    SameSingleOctetDataSource(const SameSingleOctetDataSource&) = default;

    /**
     * Copy operator (rule of 5).
     */
    SameSingleOctetDataSource& operator=(const SameSingleOctetDataSource&) = default;

    /**
     * Move operation (rule of 5).
     */
    SameSingleOctetDataSource(SameSingleOctetDataSource&&) = default;

    /**
     * Move operator (rule of 5).
     */
    SameSingleOctetDataSource& operator=(SameSingleOctetDataSource&&) = default;


  private:
    char8_t value ;
};
// ================[ END common code ]==================

// TODO : inside namespace
// ================[ BEGIN test suite ]==================
Test(VtInputSource, should_return_vt_null_on_reading_the_zero_octet) {
    cr_assert(false);
}

Test(VtInputSource, should_return_printable_characters) {
    cr_assert(false);
}

Test(VtInputSource, should_return_keys_on_recognizing_key_sequences) {
    cr_assert(false);
}

Test(VtInputSource, should_return_vt_report_on_recognizing_cursor_position_report) {
    cr_assert(false);
}

Test(VtInputSource, should_reuse_unconsumed_reads_from_the_character_data_source) {
    cr_assert(false);
}

// ================[ END test suite ]==================

