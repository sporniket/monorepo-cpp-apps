// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 David SPORN
// ---
// This is part of **io library by sporniket**.
// A collection of models to write I/O processing.
// ---

#ifndef __CMSPK__IO__DATA_SINK_HPP__
#define __CMSPK__IO__DATA_SINK_HPP__

// required
#include <expected>

#include "cmspk/io/IoError.hpp"

namespace cmspk::io {
// ================[ CODE BEGINS ]================
/**
 * Generic model of an interface to model a sink for an arbitrary type.
 *
 * @param T the type to be consumed.
 * @param CharT type of character to be used for errors, e.g. `char8_t`, `char32_t`.
 */
template <class T, class CharT>
class BasicDataSink {
  public:
    BasicDataSink() = default;
    virtual std::expected<T, BasicIoError<CharT>> next() = 0;
    virtual bool hasNext() { return true; }  // override to avoid a read expected to fail

    virtual ~BasicDataSink() = default;
    /**
     * Copy operation (rule of 5).
     */
    BasicDataSink(const BasicDataSink&) = default;

    /**
     * Copy operator (rule of 5).
     */
    BasicDataSink& operator=(const BasicDataSink&) = default;

    /**
     * Move operation (rule of 5).
     */
    BasicDataSink(BasicDataSink&&) = default;

    /**
     * Move operator (rule of 5).
     */
    BasicDataSink& operator=(BasicDataSink&&) = default;
};

/**
 * An interface to model a source of arbitrary type.
 *
 * @param T the type returned by a successfull read.
 */
template <class T>
using DataSink = BasicDataSink<T, char32_t>;

// ================[ END OF CODE ]================
}  // namespace cmspk::io
#endif
