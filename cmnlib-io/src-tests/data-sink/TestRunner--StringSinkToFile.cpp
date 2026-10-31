// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 David SPORN
// ---
// This is part of **term library by sporniket**.
// A collection of utilities for writing terminal-hosted applications.
// ---
// @since v0.0.2

// standard libs
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <vector>

// testing framework
#include <criterion/criterion.h>

// project
#include "cmspk/io/StringSinkToFile.hpp"

// ================[ BEGIN common code ]==================

struct SpyingWriteReport {
    int fileDescriptorId;
    size_t writeLength;
};

// ================[ END common code ]==================

// ================[ BEGIN test suite ]==================
// Parameterized tests are a hassle, see it later

Test(BasicStringSinkToFile, should_call_the_provided_write_function) {
    /// __given__ the string sink of ascii characters (char8_t) to file is given a dummy file descriptor and a spying write function that is successfull
    SpyingWriteReport report{.fileDescriptorId = -1, .writeLength = 0};
    auto spyingWrite = [&report](int fd, const void* buffer, size_t length) mutable noexcept -> ssize_t {
        report.fileDescriptorId = fd;
        report.writeLength = length;
        return length;
    };
    cmspk::io::BasicStringSinkToFile<char8_t> sink(0, spyingWrite);

    /// __when__ calling push(u8"hello")
    cmspk::io::BasicDataSinkReport<char8_t> writeReport = sink.push(u8"hello");

    /// __then__ it has called the write function with the dummy file decriptor, to write 5 octets
    cr_assert(report.fileDescriptorId == 0);
    cr_assert(report.writeLength == 5);

    /// __then__ it returns a report with 1 item, a size of 5, and no error
    cr_assert(writeReport.countItem == 1);
    cr_assert(writeReport.countSize == 5);
    cr_assert(not(writeReport.error));
}

Test(BasicStringSinkToFile, should_support_wide_characters_size) {
    /// __given__ the string sink of unicode characters (char32_t) to file is given a dummy file descriptor and a spying write function that is successfull
    SpyingWriteReport report{.fileDescriptorId = -1, .writeLength = 0};
    auto spyingWrite = [&report](int fd, const void* buffer, size_t length) mutable noexcept -> ssize_t {
        report.fileDescriptorId = fd;
        report.writeLength = length;
        return length;
    };
    cmspk::io::BasicStringSinkToFile<char32_t> sink(0, spyingWrite);

    /// __when__ calling push(U"hello")
    cmspk::io::BasicDataSinkReport<char32_t> writeReport = sink.push(U"hello");

    /// __then__ it has called the write function with the dummy file decriptor, to write 20 octets
    cr_assert(report.fileDescriptorId == 0);
    cr_assert(report.writeLength == 20);

    /// __then__ it returns a report with 1 item, a size of 20, and no error
    cr_assert(writeReport.countItem == 1);
    cr_assert(writeReport.countSize == 20);
    cr_assert(not(writeReport.error));
}

Test(BasicStringSinkToFile, should_support_a_write_operation_returning_zero) {
    /// __given__ the string sink of ascii characters (char8_t) to file is given a dummy file descriptor and a dummy write function that returns zero
    SpyingWriteReport report{.fileDescriptorId = -1, .writeLength = 0};
    auto spyingWrite = [&report](int fd, const void* buffer, size_t length) mutable noexcept -> ssize_t {
        report.fileDescriptorId = fd;
        report.writeLength = length;
        return 0;
    };
    cmspk::io::BasicStringSinkToFile<char8_t> sink(0, spyingWrite);

    /// __when__ calling push(u8"hello")
    cmspk::io::BasicDataSinkReport<char8_t> writeReport = sink.push(u8"hello");

    /// __then__ it returns a report with 0 item, a size of 0, and a not ready error
    cr_assert(writeReport.countItem == 0);
    cr_assert(writeReport.countSize == 0);
    cr_assert(writeReport.error);
    cr_assert(writeReport.error->type == cmspk::io::IoErrorType::NOT_READY);

    /// __then__ calling canPush() returns true
    cr_assert(sink.canPush());
}

Test(BasicStringSinkToFile, should_support_a_write_operation_returning_a_definitevely_bad_error) {
    /// __given__ the string sink of ascii characters (char8_t) to file is given a dummy file descriptor and a dummy write function that returns -1 and set
    /// errno to an error code that is __NOT__ EAGAIN or EWOULDBLOCK
    for (int errnoTest : std::vector<int>({EBADF, EDESTADDRREQ, EDQUOT, EFAULT, EFBIG, EINTR, EINVAL, EIO, ENOSPC, EPERM, EPIPE})) {
        SpyingWriteReport report{.fileDescriptorId = -1, .writeLength = 0};
        auto spyingWrite = [&report, errnoTest](int fd, const void* buffer, size_t length) mutable noexcept -> ssize_t {
            report.fileDescriptorId = fd;
            report.writeLength = length;
            errno = errnoTest;
            return -1;
        };
        cmspk::io::BasicStringSinkToFile<char8_t> sink(0, spyingWrite);

        /// __when__ calling push(u8"hello")
        cmspk::io::BasicDataSinkReport<char8_t> writeReport = sink.push(u8"hello");

        /// __then__ it returns a report with 0 item, a size of 0, and an irrecoverable error
        cr_assert(writeReport.countItem == 0);
        cr_assert(writeReport.countSize == 0);
        cr_assert(writeReport.error);
        cr_assert(writeReport.error->type == cmspk::io::IoErrorType::BAD);

        /// __then__ calling canPush() returns false
        cr_assert(not(sink.canPush()));
    }
}

Test(BasicStringSinkToFile, should_support_a_write_operation_returning_a_transient_error) {
    /// __given__ the string sink of ascii characters (char8_t) to file is given a dummy file descriptor and a dummy write function that returns -1 and set
    /// errno to an error code that __IS__ EAGAIN or EWOULDBLOCK
    for (int errnoTest : std::vector<int>({EAGAIN, EWOULDBLOCK})) {
        SpyingWriteReport report{.fileDescriptorId = -1, .writeLength = 0};
        auto spyingWrite = [&report, errnoTest](int fd, const void* buffer, size_t length) mutable noexcept -> ssize_t {
            report.fileDescriptorId = fd;
            report.writeLength = length;
            errno = errnoTest;
            return -1;
        };
        cmspk::io::BasicStringSinkToFile<char8_t> sink(0, spyingWrite);

        /// __when__ calling push(u8"hello")
        cmspk::io::BasicDataSinkReport<char8_t> writeReport = sink.push(u8"hello");

        /// __then__ it returns a report with 0 item, a size of 0, and a not ready error
        cr_assert(writeReport.countItem == 0);
        cr_assert(writeReport.countSize == 0);
        cr_assert(writeReport.error);
        cr_assert(writeReport.error->type == cmspk::io::IoErrorType::NOT_READY);

        /// __then__ calling canPush() returns true
        cr_assert(sink.canPush());
    }
}

// ================[ END test suite ]==================
