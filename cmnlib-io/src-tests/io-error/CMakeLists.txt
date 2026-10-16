# SPDX-License-Identifier: AGPL-3.0-or-later
# Copyright (C) 2026 David SPORN
# ---
# This is part of **term library by sporniket**.
# A collection of utilities for writing terminal-hosted applications.
# ---
enable_testing()

# ---
# test--io-error
# ---

# Define how to build the test suite
set(BINARY_IO_ERROR test--io-error)

# registers your cpp files from ../src
# e.g. : set(SOURCES TestRunner.cpp ../src/whatever.cpp)
set(SOURCES_IO_ERROR TestRunner--IoError.cpp)

add_executable(${BINARY_IO_ERROR} ${SOURCES_IO_ERROR})
target_include_directories(${BINARY_IO_ERROR} PUBLIC
    ../../include
)

target_link_libraries(${BINARY_IO_ERROR} PRIVATE criterion)

# ---
# Specify how to launch the test suite, and how to check that it pass
add_test(NAME DoTest--io-error--list COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${BINARY_IO_ERROR} --list --color=always)
add_test(NAME DoTest--io-error--run COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${BINARY_IO_ERROR} --color=always)

# ---
# [your next test runner here]
# ---


# ---
# Create the custom task `verify--xxx` that MUST be invoked to run this test suite.
# i.e. `cmake --build <build_dir> -- verify--xxx`
add_custom_target(
    verify--io-error COMMAND env CLICOLOR_FORCE=1 ${CMAKE_CTEST_COMMAND} --verbose
    DEPENDS
    ${BINARY_IO_ERROR}
)
