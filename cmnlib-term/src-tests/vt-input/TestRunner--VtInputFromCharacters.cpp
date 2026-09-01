// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 David SPORN
// ---
// This is part of **term library by sporniket**.
// A collection of utilities for writing terminal-hosted applications.
// ---

// standard libs
#include <cstdint>
#include <optional>
#include <variant>
#include <vector>

// testing framework
#include <criterion/criterion.h>

// project
#include "cmspk/term/VtInputFromCharacters.hpp"

// ================[ BEGIN common code ]==================
// ================[ END common code ]==================

// TODO : inside namespace
// ================[ BEGIN test suite ]==================
// Parameterized tests are a hassle, see it later
Test(VtInputFromCharacters, should_return_an_unknown_virtual_terminal_input_on_reading_an_octet_that_is_not_recognizable) {
    std::vector<char8_t> toBeTested{0, 28, 29, 30, 31};
    for (char8_t c : toBeTested) {
        cmspk::term::VtInputFromCharacters dut;
        dut.reset();
        cr_assert(dut.append(c), "Failed for input value %d", (uint8_t)c);
        cr_assert(not(dut.canAppend()), "Failed for input value %d", (uint8_t)c);
        cr_assert(dut.canGetData(), "Failed for input value %d", (uint8_t)c);
        std::optional<cmspk::term::VtInput> vtin = dut.getData();
        cr_assert(vtin, "Failed for input value %d", (uint8_t)c);
        cr_assert(std::holds_alternative<cmspk::term::VtInputUnknown>(*vtin), "Failed for input value %d", (uint8_t)c);
        cmspk::term::VtInputUnknown vtu = std::get<cmspk::term::VtInputUnknown>(*vtin);
        char8_t raw = vtu.rawValue;
        cr_assert((c == raw), "Failed for input value %d", (uint8_t)c);
    }
}
Test(VtInputFromCharacters, for_now_should_return_an_unknown_virtual_terminal_input_on_reading_any_octet) {
    for (uint16_t cint = 0; cint < 256; cint++) {
        char8_t c = (char8_t)cint;
        cmspk::term::VtInputFromCharacters dut;
        dut.reset();
        cr_assert(dut.append(c), "Failed for input value %d", (uint8_t)c);
        cr_assert(not(dut.canAppend()), "Failed for input value %d", (uint8_t)c);
        cr_assert(dut.canGetData(), "Failed for input value %d", (uint8_t)c);
        std::optional<cmspk::term::VtInput> vtin = dut.getData();
        cr_assert(vtin, "Failed for input value %d", (uint8_t)c);
        cr_assert(std::holds_alternative<cmspk::term::VtInputUnknown>(*vtin), "Failed for input value %d", (uint8_t)c);
        cmspk::term::VtInputUnknown vtu = std::get<cmspk::term::VtInputUnknown>(*vtin);
        char8_t raw = vtu.rawValue;
        cr_assert((c == raw), "Failed for input value %d", (uint8_t)c);
    }
}

// ================[ END test suite ]==================
