# SPDX-License-Identifier: AGPL-3.0-or-later
# Copyright (C) 2026 David SPORN
# ---
# This is part of **term library by sporniket**.
# A collection of utilities for writing terminal-hosted applications.
# ---


# Define how to build the test suite
set(BINARY__TTY_INPUT test--tty-input)

# registers your cpp files from ../src
# e.g. : set(SOURCES TestRunner.cpp ../src/whatever.cpp)
set(SOURCES__TTY_INPUT TestRunner.cpp)

add_executable(${BINARY__TTY_INPUT} ${SOURCES__TTY_INPUT})
target_include_directories(${BINARY__TTY_INPUT} PUBLIC
    ../../include
    ../../../cmnlib-io/include
)

target_link_libraries(${BINARY__TTY_INPUT} PRIVATE criterion)

# ---
# Create the custom task `verify--xxx` that MUST be invoked to run this test suite.
# i.e. `cmake --build <build_dir> -- verify--xxx`
add_custom_target(
    verify--tty-input COMMAND env CLICOLOR_FORCE=1 ${CMAKE_CTEST_COMMAND} --verbose
    DEPENDS ${BINARY__TTY_INPUT}
)

# ---
# Specify how to launch the test suite, and how to check that it pass
enable_testing()
add_test(NAME DoTest--tty-input COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${BINARY__TTY_INPUT} --color=always)

