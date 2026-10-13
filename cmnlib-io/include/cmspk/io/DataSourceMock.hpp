// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 David SPORN
// ---
// This is part of **io library by sporniket**.
// A collection of models to write I/O processing.
// ---

#ifndef __CMSPK__IO__DATA_SOURCE_MOCK_HPP__
#define __CMSPK__IO__DATA_SOURCE_MOCK_HPP__

// required
#include <expected>
#include <variant>
#include <vector>

#include "cmspk/io/DataSource.hpp"
#include "cmspk/io/IoError.hpp"

namespace cmspk::io {
// ================[ CODE BEGINS ]================

/**
 * Implementation mainly designed to test code using a DataSource.
 *
 * It can be created with either a vector of values (to simulate the absence of errors until the end), or a vector
 * of either values or errors (to simulate some errors happening while reading).
 *
 * ## Behaviours
 *
 * > See `TestRunner--DataSourceMock.cpp`
 *
 * @param T the type returned by a successfull read.
 * @param CharT type of character to be used for errors, e.g. `char8_t`, `char32_t`.
 */
template <class T, class CharT>
class DataSourceMock : public cmspk::io::BasicDataSource<T, CharT> {
  public:
    /**
     * Create a mock data source that will return all expected value until the end of the sequence.
     *
     * @param values the list of values to replay.
     **/
    DataSourceMock(std::vector<T> values) {
        sequence.reserve(values.size());
        for (T v : values) {
            sequence.push_back(v);
        }
        cursor = this->sequence.begin();
    }
    /**
     * Create a mock data source that will reproduce the given sequence of events.
     *
     * @param sequence the sequence of event to replay.
     */
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

// ================[ END OF CODE ]================
}  // namespace cmspk::io
#endif
