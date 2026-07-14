// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 David SPORN
// ---
// This is part of **io library by sporniket**.
// A collection of models to write I/O processing.
// ---

#ifndef __CMSPK__IO__DATA_SOURCE_HPP__
#define __CMSPK__IO__DATA_SOURCE_HPP__

// required
#include "cmspk/io/IoError.hpp"

namespace cmspk::io {
// ================[ CODE BEGINS ]================
/**
 * Generic model of an interface to model a source of arbitrary type.
 *
 * @param T the type returned by a successfull read.
 * @param CharT type of character, e.g. `char8_t`, `char32_t`.
 */
template <class T, class CharT>
class BasicDataSource {
  public:
    virtual std::expected<T, BasicIoError<CharT>> next() = 0;
    virtual bool hasNext() { return true; }  // override to avoid a read expected to fail

    virtual ~BasicDataSource() = default;
    /**
     * Copy operation (rule of 5).
     */
    BasicDataSource(const BasicDataSource&) = default;

    /**
     * Copy operator (rule of 5).
     */
    BasicDataSource& operator=(const BasicDataSource&) = default;

    /**
     * Move operation (rule of 5).
     */
    BasicDataSource(BasicDataSource&&) = default;

    /**
     * Move operator (rule of 5).
     */
    BasicDataSource& operator=(BasicDataSource&&) = default;
};

/**
 * An interface to model a source of arbitrary type.
 *
 * @param T the type returned by a successfull read.
 */
template <class T>
using DataSource = BasicDataSource<T, char32_t>;

// ================[ END OF CODE ]================
}  // namespace cmspk::io
#endif
