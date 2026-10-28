# SPDX-License-Identifier: AGPL-3.0-or-later
# Copyright (C) 2026 David SPORN
# ---
# This is part of **term library by sporniket**.
# A collection of utilities for writing terminal-hosted applications.
# ---
cmake_minimum_required(VERSION 3.31)
project(tui_sped)
set(CMAKE_CXX_STANDARD 23)
add_compile_options(-Wall -Wextra -Wpedantic)

# Generate the db for clang toolings by default
# need to call supplemental target
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
message("export compile command is ${CMAKE_EXPORT_COMPILE_COMMANDS}")

#---
add_executable(sped src-main/sped.cpp ../cmnlib-term/src/RawTerminalApp.cpp)
target_include_directories(sped PUBLIC
	../cmnlib-term/include
	../cmnlib-io/include
)


# The test suite is implemented in a dedicated source folder.
# add_subdirectory(src-tests)

# Collect files for clang-format/clang-tidy tasks
file(GLOB_RECURSE FORMATTABLE_SOURCES
       ${PROJECT_SOURCE_DIR}/include/*.hpp
       ${PROJECT_SOURCE_DIR}/src-main/*.cpp
       ${PROJECT_SOURCE_DIR}/src-tests/*.cpp
)

# Custom targets for pretty printing the sources (check and perform)
add_custom_target(check-prettier
    COMMAND clang-format
    --dry-run
    --style=file:${PROJECT_SOURCE_DIR}/clang-format.yaml
    ${FORMATTABLE_SOURCES}
)
add_custom_target(prettier
    COMMAND clang-format
    -i --verbose
    --style=file:${PROJECT_SOURCE_DIR}/clang-format.yaml
    ${FORMATTABLE_SOURCES}
)

# Linter
add_custom_target(linter
    COMMAND clang-tidy-19
    --extra-arg=-std=c++23
    --extra-arg=-I${PROJECT_SOURCE_DIR}/../cmnlib-term/include
    --extra-arg=-I${PROJECT_SOURCE_DIR}/../cmnlib-io/include
    ${FORMATTABLE_SOURCES}
    DEPENDS prettier # refresh-compile-db
)

# Documentation
add_custom_target(doc-generate
    COMMAND doxygen
    ../doxyfile-format.toml
    DEPENDS prettier
)

### For later

# Custom target to manage compile_commands.json
# requires compdb : https://github.com/Sarcasm/compdb
# add_custom_target(refresh-compile-db
#    COMMAND compdb
#    -p ${CMAKE_BINARY_DIR}
#    list
#    > ${PROJECT_SOURCE_DIR}/compile_commands.json
# )


