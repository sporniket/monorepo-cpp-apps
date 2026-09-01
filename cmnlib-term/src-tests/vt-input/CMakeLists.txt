# SPDX-License-Identifier: AGPL-3.0-or-later
# Copyright (C) 2026 David SPORN
# ---
# This is part of **term library by sporniket**.
# A collection of utilities for writing terminal-hosted applications.
# ---


# Define how to build the test suite
set(BINARY_VT_INPUT_FROM_CHARACTERS test--vt-input-from-characters)

# registers your cpp files from ../src
# e.g. : set(SOURCES TestRunner.cpp ../src/whatever.cpp)
set(SOURCES_VT_INPUT_FROM_CHARACTERS TestRunner--VtInputFromCharacters.cpp)

add_executable(${BINARY_VT_INPUT_FROM_CHARACTERS} ${SOURCES_VT_INPUT_FROM_CHARACTERS})
target_include_directories(${BINARY_VT_INPUT_FROM_CHARACTERS} PUBLIC
    ../../include
    ../../../cmnlib-io/include
)

target_link_libraries(${BINARY_VT_INPUT_FROM_CHARACTERS} PRIVATE criterion)

# ---
# Create the custom task `verify--xxx` that MUST be invoked to run this test suite.
# i.e. `cmake --build <build_dir> -- verify--xxx`
add_custom_target(
    verify--vt-input-from-characters COMMAND env CLICOLOR_FORCE=1 ${CMAKE_CTEST_COMMAND} --verbose
    DEPENDS ${BINARY_VT_INPUT_FROM_CHARACTERS}
)

# ---
# Specify how to launch the test suite, and how to check that it pass
enable_testing()
add_test(NAME DoTest--vt-input-from-characters COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${BINARY_VT_INPUT_FROM_CHARACTERS} --list --color=always)

