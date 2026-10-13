# SPDX-License-Identifier: AGPL-3.0-or-later
# Copyright (C) 2026 David SPORN
# ---
# This is part of **term library by sporniket**.
# A collection of utilities for writing terminal-hosted applications.
# ---
enable_testing()

# ---
# test--data-source-mock
# ---

# Define how to build the test suite
set(BINARY_DATA_SOURCE_MOCK test--data-source-mock)

# registers your cpp files from ../src
# e.g. : set(SOURCES TestRunner.cpp ../src/whatever.cpp)
set(SOURCES_DATA_SOURCE_MOCK TestRunner--DataSourceMock.cpp)

add_executable(${BINARY_DATA_SOURCE_MOCK} ${SOURCES_DATA_SOURCE_MOCK})
target_include_directories(${BINARY_DATA_SOURCE_MOCK} PUBLIC
    ../../include
)

target_link_libraries(${BINARY_DATA_SOURCE_MOCK} PRIVATE criterion)

# ---
# Specify how to launch the test suite, and how to check that it pass
add_test(NAME DoTest--data-source-mock--list COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${BINARY_DATA_SOURCE_MOCK} --list --color=always)
add_test(NAME DoTest--data-source-mock--run COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${BINARY_DATA_SOURCE_MOCK} --color=always)

# ---
# [your next test runner here]
# ---


# ---
# Create the custom task `verify--xxx` that MUST be invoked to run this test suite.
# i.e. `cmake --build <build_dir> -- verify--xxx`
add_custom_target(
    verify--data-source COMMAND env CLICOLOR_FORCE=1 ${CMAKE_CTEST_COMMAND} --verbose
    DEPENDS
    ${BINARY_DATA_SOURCE_MOCK}
)
