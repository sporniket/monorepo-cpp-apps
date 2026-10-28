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
 * Generic model of C file encapsulation to read data.
 *
 *
 * @param T the type returned by a successfull read.
 * @param CharT type of character to be used for errors, e.g. `char8_t`, `char32_t`.
 * @since v0.0.2
 */
template <class T, class CharT>
class BasicDataSourceFromFile : public cmspk::io::BasicDataSource<T, CharT> {
  public:
    BasicDataSourceFromFile(int fileDescriptorId, std::function<ssize_t(int, void*, size_t)> readImpl = read)
        : fileDescriptorId(fileDescriptorId), readImpl(readImpl) {}
    virtual std::expected<char8_t, cmspk::io::BasicIoError<CharT>> next() {
        if (irrecoverable) {
            return std::unexpected(BasicDataSourceFromFile::irrecoverable_error());
        }
        ssize_t result = readImpl(fileDescriptorId, &c, sizeof(T));
        int savedErrno = errno;
        if (result < 0) {
            if (savedErrno != EAGAIN && savedErrno != EWOULDBLOCK) {
                irrecoverable = true;
                return std::unexpected(BasicDataSourceFromFile::irrecoverable_error());
            }
            return std::unexpected(BasicDataSourceFromFile::not_ready_error());
        } else if (result == 0) {
            return std::unexpected(BasicDataSourceFromFile::not_ready_error());
        }
        return c;
    }
    virtual bool hasNext() { return !irrecoverable; }

    virtual ~BasicDataSourceFromFile() {}
    /**
     * Copy operation (rule of 5).
     */
    BasicDataSourceFromFile(const BasicDataSourceFromFile&) = default;

    /**
     * Copy operator (rule of 5).
     */
    BasicDataSourceFromFile& operator=(const BasicDataSourceFromFile&) = default;

    /**
     * Move operation (rule of 5).
     */
    BasicDataSourceFromFile(BasicDataSourceFromFile&&) = default;

    /**
     * Move operator (rule of 5).
     */
    BasicDataSourceFromFile& operator=(BasicDataSourceFromFile&&) = default;

  private:
    int fileDescriptorId;
    T c;
    bool irrecoverable = false;
    std::function<ssize_t(int, void*, size_t)> readImpl;
    static constexpr std::basic_string<CharT> message_bad_state = convertUsAsciiString<CharT>(u8"bad.state");
    static constexpr std::basic_string<CharT> message_not_ready = convertUsAsciiString<CharT>(u8"try.later");
    static const cmspk::io::BasicIoError<CharT> irrecoverable_error() {
        return {.type = cmspk::io::IoErrorType::BAD, .message = message_bad_state, .details = {}};
    }
    static const cmspk::io::BasicIoError<CharT> not_ready_error() {
        return {.type = cmspk::io::IoErrorType::NOT_READY, .message = message_not_ready, .details = {}};
    }
};

/**
 * An interface to encapsulate a C file to read data.
 *
 * @param T the type returned by a successfull read.
 * @since v0.0.2
 */
template <class T>
using DataSourceFromFile = BasicDataSourceFromFile<T, char32_t>;

// ================[ END OF CODE ]================
}  // namespace cmspk::io
#endif
