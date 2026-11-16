# SPDX-License-Identifier: AGPL-3.0-or-later
# Copyright (C) 2026 David SPORN
# ---
# This is part of **txdos library by sporniket**.
# A collection of utilities for writing terminal-hosted applications.
# ---
enable_testing()

# ---
# test--io-error
# ---

# Define how to build the test suite
set(BINARY_HISTORY specs--history)

# registers your cpp files from ../src
# e.g. : set(SOURCES TestRunner.cpp ../src/whatever.cpp)
set(SOURCES_HISTORY Specs--History.cpp)

add_executable(${BINARY_HISTORY} ${SOURCES_HISTORY})
target_include_directories(${BINARY_HISTORY} PUBLIC
    ../../include
)

target_link_libraries(${BINARY_HISTORY} PRIVATE Catch2Main Catch2)

# ---
# Specify how to launch the test suite, and how to check that it pass
add_test(NAME run--${BINARY_HISTORY}
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${BINARY_HISTORY}
    --success
    --colour-mode ansi
    --verbosity quiet
    #--list-reporters
    #--reporter Automake
    #--reporter compact
    --reporter console
    #--reporter JSON
    #--reporter JUnit
    #--reporter SonarQube
    #--reporter TAP
    #--reporter TeamCity
    #--reporter XML
)

# ---
# [your next test runner here]
# ---


# ---
# Create the custom task `verify--xxx` that MUST be invoked to run this test suite.
# i.e. `cmake --build <build_dir> -- verify--xxx`
add_custom_target(
    verify--history COMMAND env CLICOLOR_FORCE=1 ${CMAKE_CTEST_COMMAND} --verbose
    DEPENDS
    ${BINARY_HISTORY}
)
