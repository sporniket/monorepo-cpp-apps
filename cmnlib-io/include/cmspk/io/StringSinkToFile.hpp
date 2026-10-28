// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 David SPORN
// ---
// This is part of **io library by sporniket**.
// A collection of models to write I/O processing.
// ---

#ifndef __CMSPK__IO__STRING_SINK_TO_FILE_HPP__
#define __CMSPK__IO__STRING_SINK_TO_FILE_HPP__

// required
#include <unistd.h>

#include <optional>
#include <functional>

#include "cmspk/io/DataSink.hpp"

namespace cmspk::io {
// ================[ CODE BEGINS ]================
/**
 * Sink for strings, encapsulating a C file write operation.
 */
template<class CharT>
class BasicStringSinkToFile : public cmspk::io::BasicDataSink<std::basic_string<CharT>, CharT> {
  public:
    BasicStringSinkToFile(int fileDescriptorId, std::function<ssize_t(int, const void*, size_t)> writeImpl = write)
        : fileDescriptorId(fileDescriptorId), writeImpl(writeImpl) {}

    virtual BasicDataSinkReport<CharT> push(const std::basic_string<CharT>& value) {
        if (irrecoverable) {
            return {.countItem = 0, .countSize = 0, .error = irrecoverable_error()};
        }
        ssize_t result = writeImpl(fileDescriptorId, value.c_str(), value.size() * sizeof(CharT));
        return makeReportForSingleOperation(result, errno);
    }

    virtual bool canPush() { return !irrecoverable; }

    virtual ~BasicStringSinkToFile() {}
    /**
     * Copy operation (rule of 5).
     */
    BasicStringSinkToFile(const BasicStringSinkToFile&) = default;

    /**
     * Copy operator (rule of 5).
     */
    BasicStringSinkToFile& operator=(const BasicStringSinkToFile&) = default;

    /**
     * Move operation (rule of 5).
     */
    BasicStringSinkToFile(BasicStringSinkToFile&&) = default;

    /**
     * Move operator (rule of 5).
     */
    BasicStringSinkToFile& operator=(BasicStringSinkToFile&&) = default;

  private:
    int fileDescriptorId;
    bool irrecoverable = false;
    std::function<ssize_t(int, const void*, size_t)> writeImpl;
    static constexpr std::basic_string<CharT> message_bad_state = convertUsAsciiString<CharT>(u8"bad.state");
    static constexpr std::basic_string<CharT> message_not_ready = convertUsAsciiString<CharT>(u8"try.later");
    static constexpr std::basic_string<CharT> message_null_data = convertUsAsciiString<CharT>(u8"data.is.null");
    static const cmspk::io::BasicIoError<CharT> irrecoverable_error() {
        return {.type = cmspk::io::IoErrorType::BAD, .message = message_bad_state, .details = {}};
    }
    static const cmspk::io::BasicIoError<CharT> not_ready_error() {
        return {.type = cmspk::io::IoErrorType::NOT_READY, .message = message_not_ready, .details = {}};
    }
    static const cmspk::io::BasicIoError<CharT> null_data_error() {
        // FIXME : provision a new error type
        return {.type = cmspk::io::IoErrorType::NOT_READY, .message = message_null_data, .details = {}};
    }

    BasicDataSinkReport<CharT> makeReportForSingleOperation(ssize_t writeResult, int savedErrno) {
        if (writeResult < 0) {
            if (savedErrno != EAGAIN && savedErrno != EWOULDBLOCK) {
                irrecoverable = true;
                return {.countItem = 0, .countSize = 0, .error = BasicStringSinkToFile<CharT>::irrecoverable_error()};
            }
            return {.countItem = 0, .countSize = 0, .error = BasicStringSinkToFile<CharT>::not_ready_error()};
        } else if (writeResult == 0) {
            return {.countItem = 0, .countSize = 0, .error = BasicStringSinkToFile<CharT>::not_ready_error()};
        }
        return {.countItem = 1, .countSize = (size_t)writeResult, .error = std::nullopt };
    }
};

// ================[ END OF CODE ]================
}  // namespace cmspk::io
#endif
