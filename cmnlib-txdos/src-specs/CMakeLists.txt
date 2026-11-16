# SPDX-License-Identifier: AGPL-3.0-or-later
# Copyright (C) 2026 David SPORN
# ---
# This is part of **txdos library by sporniket**.
# An operating system for textual documents.
# ---

# Uses catch2 test library
find_library(LIBCATCH2MAIN Catch2Main REQUIRED)

# each module has its own folder
add_subdirectory(history)

# ---
# Create the custom task `verify` that MUST be invoked to run this test suite.
# i.e. `cmake --build <build_dir> -- verify`
add_custom_target(
    verify
    DEPENDS
    verify--history
)

