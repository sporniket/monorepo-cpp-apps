// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 David SPORN
// ---
// This is part of **io library by sporniket**.
// A collection of models to write I/O processing.
// ---

#ifndef __CMSPK__IO__IO_ERROR_HPP__
#define __CMSPK__IO__IO_ERROR_HPP__

#include <cstdint>
#include <string>
#include <vector>

namespace cmspk::io {
// ================[ CODE BEGINS ]================
/**
 * Qualifier of the kind of I/O error.
 *
 * @since v0.0.1
 */
enum class IoErrorType {
    /**
     * It is possible to retry later and get a success.
     */
    NOT_READY,
    /**
     * There is no more data to read.
     */
    END_OF_DATA,
    /**
     * It is impossible to get a succesfull read/write.
     */
    BAD
};

/**
 * An I/O error is described with a _type_ (a.k.a. kind/class/family/...), a message, and some additional details.
 *
 * By convention, the message will be a key to retrieve a localized message.
 *
 * Also, the message and additional details could be represented as US-ASCII strings or Unicode strings.
 *
 * @param CharT the type to use as character, e.g. `char8_t`, `char32_t`.
 * @since v0.0.1
 */
template <class CharT>
struct BasicIoError {
    IoErrorType type;
    std::basic_string<CharT> message;
    std::vector<std::basic_string<CharT>> details;
};

/**
 * I/O error to use when using US-ASCII strings.
 * @since v0.0.1
 */
using IoErrorAscii = BasicIoError<char8_t>;

/**
 * I/O error to use when using Unicode strings.
 * @since v0.0.1
 */
using IoError = BasicIoError<char32_t>;

// TODO -- move into its own lib
/**
 * Create a string from a string containing only US-ASCII characters.
 *
 * Only printable US-ASCII characters are copied, non US-ASCII characters and non printable characters are replaced by '?' (ascii code 63).
 *
 * ## Behaviour
 *
 * > See `TestRunner--convertUsAsciiString.cpp`
 *
 * @param CharT the target character type.
 * @param source the us-ascii string to convert.
 * @since v0.0.2
 */
template <class CharT>
constexpr std::basic_string<CharT> convertUsAsciiString(const std::basic_string<char8_t>& source) {
    std::basic_string<CharT> recipient;
    recipient.reserve(source.size());
    for (char8_t c : source) {
        recipient.push_back(((((uint8_t)c) < 127) && (((uint8_t)c) >= 32)) ? c : 63);
    }

    return recipient;
}

/**
 * Create a string from a string containing only US-ASCII and serial control (range(0,32)) characters.
 *
 * Non US-ASCII characters are replaced by '?' (ascii code 63).
 *
 * ## Behaviour
 *
 * > See `TestRunner--convertUsAsciiString.cpp`
 *
 * @param CharT the target character type.
 * @param source the us-ascii string to convert.
 * @since v0.0.2
 */
template <class CharT>
constexpr std::basic_string<CharT> convertUsAsciiAndControlString(const std::basic_string<char8_t>& source) {
    std::basic_string<CharT> recipient;
    recipient.reserve(source.size());
    for (char8_t c : source) {
        recipient.push_back(((((uint8_t)c) < 127)) ? c : 63);
    }

    return recipient;
}
// ================[ END OF CODE ]================
}  // namespace cmspk::io
#endif
