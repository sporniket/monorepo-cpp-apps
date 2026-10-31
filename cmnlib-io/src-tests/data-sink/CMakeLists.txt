# SPDX-License-Identifier: AGPL-3.0-or-later
# Copyright (C) 2026 David SPORN
# ---
# This is part of **term library by sporniket**.
# A collection of utilities for writing terminal-hosted applications.
# ---
enable_testing()

# ---
# test--string-sink-to-file
# ---

# Define how to build the test suite
set(BINARY_STRING_SINK_TO_FILE test--string-sink-to-file)

# registers your cpp files from ../src
# e.g. : set(SOURCES TestRunner.cpp ../src/whatever.cpp)
set(SOURCES_STRING_SINK_TO_FILE TestRunner--StringSinkToFile.cpp)

add_executable(${BINARY_STRING_SINK_TO_FILE} ${SOURCES_STRING_SINK_TO_FILE})
target_include_directories(${BINARY_STRING_SINK_TO_FILE} PUBLIC
    ../../include
)

target_link_libraries(${BINARY_STRING_SINK_TO_FILE} PRIVATE criterion)

# ---
# Specify how to launch the test suite, and how to check that it pass
add_test(NAME DoTest--string-sink-to-file--list COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${BINARY_STRING_SINK_TO_FILE} --list --color=always)
add_test(NAME DoTest--string-sink-to-file--run COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${BINARY_STRING_SINK_TO_FILE} --color=always)

# ---
# [your next test runner here]
# ---


# ---
# Create the custom task `verify--xxx` that MUST be invoked to run this test suite.
# i.e. `cmake --build <build_dir> -- verify--xxx`
add_custom_target(
    verify--data-sink COMMAND env CLICOLOR_FORCE=1 ${CMAKE_CTEST_COMMAND} --verbose
    DEPENDS
    ${BINARY_STRING_SINK_TO_FILE}
)
