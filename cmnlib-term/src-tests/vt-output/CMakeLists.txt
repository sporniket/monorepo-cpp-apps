# SPDX-License-Identifier: AGPL-3.0-or-later
# Copyright (C) 2026 David SPORN
# ---
# This is part of **term library by sporniket**.
# A collection of utilities for writing terminal-hosted applications.
# ---
enable_testing()

# ---
# test--vt-output-builder
# ---

# Define how to build the test suite
set(BINARY_VT_OUTPUT_BUILDER test--vt-output-builder)

# registers your cpp files from ../src
# e.g. : set(SOURCES TestRunner.cpp ../src/whatever.cpp)
set(SOURCES_VT_OUTPUT_BUILDER TestRunner--VtOutputBuilder.cpp)

add_executable(${BINARY_VT_OUTPUT_BUILDER} ${SOURCES_VT_OUTPUT_BUILDER})
target_include_directories(${BINARY_VT_OUTPUT_BUILDER} PUBLIC
    ../../include
    ../../../cmnlib-io/include
)

target_link_libraries(${BINARY_VT_OUTPUT_BUILDER} PRIVATE criterion)

# ---
# Specify how to launch the test suite, and how to check that it pass
add_test(NAME DoTest--vt-output-builder--list COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${BINARY_VT_OUTPUT_BUILDER} --list --color=always)
add_test(NAME DoTest--vt-output-builder--run COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${BINARY_VT_OUTPUT_BUILDER} --color=always)


# ---
# YOUR NEXT TEST RUNNER HERE
# ---

# ---
# Create the custom task `verify--xxx` that MUST be invoked to run this test suite.
# i.e. `cmake --build <build_dir> -- verify--xxx`
add_custom_target(
    verify--vt-output COMMAND env CLICOLOR_FORCE=1 ${CMAKE_CTEST_COMMAND} --verbose
    DEPENDS
    ${BINARY_VT_OUTPUT_BUILDER}
)
