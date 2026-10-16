// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 David SPORN
// ---
// This is part of **io library by sporniket**.
// A collection of models to write I/O processing.
// ---

#ifndef __CMSPK__IO__DATA_SOURCE_FROM_FILE_HPP__
#define __CMSPK__IO__DATA_SOURCE_FROM_FILE_HPP__

// required
#include <unistd.h>

#include <expected>
#include <functional>

#include "cmspk/io/DataSource.hpp"

namespace cmspk::io {
// ================[ CODE BEGINS ]================
/**
 * Encapsulate a C file to read data.
 *
 *
 * @param T the type returned by a successfull read.
 * @param CharT type of character to be used for errors, e.g. `char8_t`, `char32_t`.
 */
template <class T, class CharT>
class DataSourceFromFile : public cmspk::io::BasicDataSource<T, CharT> {
  public:
    DataSourceFromFile(int fileDescriptorId, std::function<size_t(int, void*, size_t)> readImpl = read)
        : fileDescriptorId(fileDescriptorId), readImpl(readImpl) {}
    virtual std::expected<char8_t, cmspk::io::BasicIoError<CharT>> next() {
        if (irrecoverable) {
            return std::unexpected(DataSourceFromFile::irrecoverable_error());
        }
        int result = readImpl(fileDescriptorId, &c, sizeof(T));
        int savedErrno = errno;
        if (result < 0) {
            if (savedErrno != EAGAIN && savedErrno != EWOULDBLOCK) {
                irrecoverable = true;
                return std::unexpected(DataSourceFromFile::irrecoverable_error());
            }
            return std::unexpected(DataSourceFromFile::not_ready_error());
        } else if (result == 0) {
            return std::unexpected(DataSourceFromFile::not_ready_error());
        }
        return c;
    }
    virtual bool hasNext() { return !irrecoverable; }

    virtual ~DataSourceFromFile() {}
    /**
     * Copy operation (rule of 5).
     */
    DataSourceFromFile(const DataSourceFromFile&) = default;

    /**
     * Copy operator (rule of 5).
     */
    DataSourceFromFile& operator=(const DataSourceFromFile&) = default;

    /**
     * Move operation (rule of 5).
     */
    DataSourceFromFile(DataSourceFromFile&&) = default;

    /**
     * Move operator (rule of 5).
     */
    DataSourceFromFile& operator=(DataSourceFromFile&&) = default;

  private:
    int fileDescriptorId;
    T c;
    bool irrecoverable = false;
    std::function<ssize_t(int, void*, size_t)> readImpl;
    static const cmspk::io::BasicIoError<CharT> irrecoverable_error() {
        return {.type = cmspk::io::IoErrorType::BAD, .message = convertUsAsciiString<CharT>(u8"bad.state"), .details = {}};
    }
    static const cmspk::io::BasicIoError<CharT> not_ready_error() {
        return {.type = cmspk::io::IoErrorType::NOT_READY, .message = convertUsAsciiString<CharT>(u8"try.later"), .details = {}};
    }
};

// ================[ END OF CODE ]================
}  // namespace cmspk::io
#endif
