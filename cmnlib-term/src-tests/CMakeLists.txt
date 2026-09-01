# SPDX-License-Identifier: AGPL-3.0-or-later
# Copyright (C) 2026 David SPORN
# ---
# This is part of **term library by sporniket**.
# A collection of utilities for writing terminal-hosted applications.
# ---

# Uses criterion, see https://github.com/Snaipe/Criterion
find_library(LIBCRITERION criterion REQUIRED)

# each module has its own folder
add_subdirectory(vt-input)

# ---
# Create the custom task `verify` that MUST be invoked to run this test suite.
# i.e. `cmake --build <build_dir> -- verify`
add_custom_target(
    verify
    DEPENDS
    verify--vt-input-from-characters
)

