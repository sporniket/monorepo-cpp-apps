# SPDX-License-Identifier: AGPL-3.0-or-later
# Copyright (C) 2026 David SPORN
# ---
# This is part of **term library by sporniket**.
# A collection of utilities for writing terminal-hosted applications.
# ---

# --- demo-bare ---
add_executable(demo-bare src/demo-bare.cpp ../src/RawTerminalApp.cpp)
target_include_directories(demo-bare PUBLIC ../include)

# --- demo-dumpkeys ---
add_executable(demo-dumpkeys src/demo-dumpkeys.cpp ../src/RawTerminalApp.cpp)
target_include_directories(demo-dumpkeys PUBLIC ../include)

