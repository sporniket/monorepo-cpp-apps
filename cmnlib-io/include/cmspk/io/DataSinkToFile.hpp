// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 David SPORN
// ---
// This is part of **io library by sporniket**.
// A collection of models to write I/O processing.
// ---

#ifndef __CMSPK__IO__DATA_SINK_TO_FILE_HPP__
#define __CMSPK__IO__DATA_SINK_TO_FILE_HPP__

// required
#include <unistd.h>

#include <expected>
#include <functional>

#include "cmspk/io/DataSink.hpp"

namespace cmspk::io {
// ================[ CODE BEGINS ]================
/**
 * Generic model of C file encapsulation to write data.
 *
 *
 * @param T the type to be written.
 * @param CharT type of character to be used for errors, e.g. `char8_t`, `char32_t`.
 * @since v0.0.2
 */
template <class T, class CharT>
class BasicDataSinkToFile : public cmspk::io::BasicDataSink<T, CharT> {
  public:
    BasicDataSinkToFile(int fileDescriptorId, std::function<size_t(int, void*, size_t)> writeImpl = write)
        : fileDescriptorId(fileDescriptorId), writeImpl(writeImpl) {}
    virtual std::expected<char8_t, cmspk::io::BasicIoError<CharT>> next() {
        if (irrecoverable) {
            return std::unexpected(BasicDataSinkToFile::irrecoverable_error());
        }
        int result = writeImpl(fileDescriptorId, &c, sizeof(T));
        int savedErrno = errno;
        if (result < 0) {
            if (savedErrno != EAGAIN && savedErrno != EWOULDBLOCK) {
                irrecoverable = true;
                return std::unexpected(BasicDataSinkToFile::irrecoverable_error());
            }
            return std::unexpected(BasicDataSinkToFile::not_ready_error());
        } else if (result == 0) {
            return std::unexpected(BasicDataSinkToFile::not_ready_error());
        }
        return c;
    }
    virtual bool hasNext() { return !irrecoverable; }

    virtual ~BasicDataSinkToFile() {}
    /**
     * Copy operation (rule of 5).
     */
    BasicDataSinkToFile(const BasicDataSinkToFile&) = default;

    /**
     * Copy operator (rule of 5).
     */
    BasicDataSinkToFile& operator=(const BasicDataSinkToFile&) = default;

    /**
     * Move operation (rule of 5).
     */
    BasicDataSinkToFile(BasicDataSinkToFile&&) = default;

    /**
     * Move operator (rule of 5).
     */
    BasicDataSinkToFile& operator=(BasicDataSinkToFile&&) = default;

  private:
    int fileDescriptorId;
    T c;
    bool irrecoverable = false;
    std::function<ssize_t(int, void*, size_t)> writeImpl;
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
 * An interface to encapsulate a C file to write data.
 *
 * @param T the type to be written.
 * @since v0.0.2
 */
template<class T>
using DataSinkToFile = BasicDataSinkToFile<T, char32_t>;

// ================[ END OF CODE ]================
}  // namespace cmspk::io
#endif
