// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 David SPORN
// ---
// This is part of **txdos library by sporniket**.
// An operating system for textual documents.
// ---

#ifndef __CMSPK__TXDOS__HISTORY_HPP__
#define __CMSPK__TXDOS__HISTORY_HPP__

// standard libs
#include <cstdint>
#include <expected>
#include <vector>
// project

namespace cmspk::txdos {
// ================[ CODE BEGINS ]================
/**
 * An history system to implement Undo/Redo functions.
 *
 * An history records the evolution of some data, it can rewind ("undo"),
 * return forward ("redo"), and when recording a new state of the data,
 * data that could be returned forward to disappear.
 *
 * An history cannot grow beyond its initial capacity.
 *
 * @see Specs--History.cpp
 *
 * @since v0.0.4
 */
template <class RecordT>
class History {
  public:
    // nested types
    /**
     * Enumerated types of errors, to qualify an error.
     */
    enum class ErrorType { out_of_range, undefined };

    /**
     * Error struct returned by an operation when it cannot perform as expected.
     */
    struct Error {
        ErrorType type;
    };

    /**
     * Shorter alias for `expected<void,E>`.
     */
    using ExpectedVoid = std::expected<void, Error>;

    /**
     * Shorter alias for `expected<Const RecordT*,E>`.
     */
    using ExpectedRecordPtr = std::expected<const RecordT*, Error>;

    /**
     * Model of the history cursor, because I need a `-1` value, and the same reach than size_t.
     */
    class Cursor {
      public:
        /**
         * Comparison implementation.
         *
         * @param x the value to compare against.
         *
         * @return `true` when the cursor is _strictly lower_ than `x`
         */
        bool isLowerThan(size_t x) const noexcept {
            if (x == 0) {
                return negative;
            }
            return negative || value < x;
        }

        /**
         * Move one step forward.
         *
         * @return either void (expected), or an error.
         */
        ExpectedVoid increment() noexcept {
            if (value == SIZE_MAX - 1) return std::unexpected(Error{.type = ErrorType::out_of_range});
            if (negative) {
                value = 0;
                negative = false;
            } else {
                ++value;
            }
            return ExpectedVoid();
        }

        /**
         * Move one step backward.
         *
         * @return either void (expected), or an error.
         */
        ExpectedVoid decrement() noexcept {
            if (negative) return std::unexpected(Error{.type = ErrorType::out_of_range});
            if (value == 0) {
                negative = true;
            } else {
                --value;
            }
            return ExpectedVoid();
        }

        /**
         * Returns the value part of cursor, meaningfull only when it is not negative (`this->isLowerThan(0) == false`).
         */
        size_t getValue() const noexcept { return value; }

      private:
        /**
         * The value, meaningfull only when it is not negative (`this->isLowerThan(0) == false`).
         */
        size_t value = 0;

        /**
         * When true, it means the cursor represents `-1`.
         */
        bool negative = true;
    };

    // constructors
    /**
     * Create an history with the given capacity (cannot be zero).
     *
     * @param capacity the wanted capacity, silently forced to one if the given capacity is zero.
     */
    History(size_t capacity = 100) : records() { records.reserve((capacity < 1) ? 1 : capacity); }
    // rule of zero

    // container methods
    /**
     * Predicate on the emptyness of the history.
     *
     * @returns `true` when history contains no records.
     */
    bool empty() const noexcept { return actualSize == 0; }

    /**
     * Assesses the number of items stored in the history.
     *
     * @returns the number of items in the history.
     */
    size_t size() const noexcept { return actualSize; }

    /**
     * Returns how much items at most it can contains.
     *
     * @returns the capacity of this history.
     */
    size_t capacity() const noexcept { return records.capacity(); }
    // I/O methods

    /**
     * Store the given item.
     *
     * @param value the record to store.
     *
     * @return either void (expected), or an error.
     */
    ExpectedVoid record(const RecordT& value) noexcept {
        if (currentHistoryCursor.isLowerThan(size() - 1)) {
            // invalidate records post cursor
            if (currentHistoryCursor.isLowerThan(0)) {
                // clear the history
                nextRecordIndex = oldestIndex;
                actualSize = 0;
            } else {
                actualSize = currentHistoryCursor.getValue() + 1;
                nextRecordIndex = wrapAround(oldestIndex + actualSize);
            }
        }
        if (nextRecordIndex < records.size()) {
            // replace existing value
            if (isFull()) {
                // forget the oldest element
                oldestIndex = increment(oldestIndex);
                --actualSize;
                // adjust cursor
                currentHistoryCursor.decrement();
            }
            records.at(nextRecordIndex) = value;
            currentHistoryCursor.increment();
            nextRecordIndex = increment(nextRecordIndex);
        } else {
            // insert
            records.push_back(value);
            nextRecordIndex = wrapAround(records.size());
            currentHistoryCursor.increment();
        }
        ++actualSize;
        return ExpectedVoid();
    }

    /**
     * Retrieve the item pointed to by the cursor of this history.
     *
     * @return either the pointer to the record (expected), or an error.
     */
    ExpectedRecordPtr recall() const noexcept {
        if (empty() || currentHistoryCursor.isLowerThan(0)) {
            return std::unexpected(Error{.type = ErrorType::out_of_range});
        }
        return &(records.at(wrapAround(oldestIndex + currentHistoryCursor.getValue())));
    }

    /**
     * Predicate.
     *
     * @returns `true` when `goBack()` will succeed.
     */
    bool canGoBack() const noexcept { return !(currentHistoryCursor.isLowerThan(0)); }

    /**
     * Move the cursor to the previous record, if possible.
     *
     * @return either void (expected), or an error.
     */
    ExpectedVoid goBack() noexcept {
        if (!canGoBack()) return std::unexpected(Error{.type = ErrorType::out_of_range});
        return currentHistoryCursor.decrement();
    }

    /**
     * Predicate.
     *
     * @returns `true` when `goForward()` will succeed.
     */
    bool canGoForward() const noexcept { return currentHistoryCursor.isLowerThan(size() - 1); }

    /**
     * Move the cursor to the next record, if possible.
     *
     * @return either void (expected), or an error.
     */
    ExpectedVoid goForward() noexcept {
        if (!canGoForward()) return std::unexpected(Error{.type = ErrorType::out_of_range});
        return currentHistoryCursor.increment();
    }

  private:
    std::vector<RecordT> records;

    /**
     * Predicate.
     *
     * @returns `true` when it contains as much element as its capacity.
     */
    bool isFull() { return size() == capacity(); }

    /**
     * REALLY NEEDED ?? Cyclic arithmetics helper.
     *
     * @returns either the next record index as is, or with capacity added to stay _after_ `oldestIndex`.
     */
    size_t adjustNextRecordIndex() const noexcept { return (oldestIndex > nextRecordIndex) ? capacity() + nextRecordIndex : nextRecordIndex; }

    /**
     * Cyclic arithmetics helper, performs the modulo operation to stay within `range(0,capacity())`.
     *
     * @param x the value to wrap around.
     *
     * @returns the modulo of the given value.
     */
    size_t wrapAround(size_t x) const noexcept { return x % capacity(); }

    /**
     * Cyclic arithmetics helper, increment the given value and wrap the result inside `range(0,capacity())`.
     *
     * @param x the value to increment and wrap around.
     *
     * @returns the incremented value.
     */
    size_t increment(size_t x) const noexcept { return wrapAround(x + 1); }

    /**
     * We have to manage the size ourself.
     *
     * Because there's an ambiguity when nextRecordIndex == oldestIndex. Is it empty or full ?
     */
    size_t actualSize = 0;

    /**
     * When history is not empty, points to the index of the oldest element.
     */
    size_t oldestIndex = 0;

    /**
     * Value between -1 and size() - 1. When greater or equal to zero, points to the index of the element to be returned by `recall()`, relative to oldestIndex.
     *
     * Visualisation : the cursor is __between__ elements, and its value points to its left.
     */
    Cursor currentHistoryCursor;

    /**
     * Points to the index at which the next element will be recorded.
     *
     * Will be in `range(0,capacity()+1)`.
     */
    size_t nextRecordIndex = 0;
};

// ================[ END OF CODE ]================
}  // namespace cmspk::txdos
#endif
