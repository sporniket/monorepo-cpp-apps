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

// ================[ BEGIN test suite ]==================
// Parameterized tests are a hassle, see it later
Test(VtInputFromCharacters, should_return_an_unknown_virtual_terminal_input_on_reading_an_octet_that_is_not_recognizable) {
    /// > This is the fall-back behaviour. The list [28,29,30,31] is what remains unknown when all other behaviours have been implemented.
    /// __for__ any character _c_ in `[28,29,30,31]`
    std::vector<char8_t> toBeTested{28, 29, 30, 31};
    for (char8_t c : toBeTested) {
        cmspk::term::VtInputFromCharacters dut;

        /// __given__ VtInputFromCharacters has been reset
        dut.reset();

        /// __when__ VtInputFromCharacters is fed with _c_
        cr_assert(dut.append(c), "Failed for input value %d", (uint16_t)c);

        /// __then__ VtInputFromCharacters does not accept characters anymore
        cr_assert(not(dut.canAppend()), "Failed for input value %d", (uint16_t)c);

        /// __then__ VtInputFromCharacters does have data
        cr_assert(dut.canGetData(), "Failed for input value %d", (uint16_t)c);

        /// __then__ the VtInputFromCharacters will return a `std::variant` containing a `VtInputUnkown` with _c_ as `rawValue`.
        std::optional<cmspk::term::VtInput> vtin = dut.getData();
        cr_assert(vtin, "Failed for input value %d", (uint16_t)c);
        cr_assert(std::holds_alternative<cmspk::term::VtInputUnknown>(*vtin), "Failed for input value %d", (uint16_t)c);
        cmspk::term::VtInputUnknown vtu = std::get<cmspk::term::VtInputUnknown>(*vtin);
        char8_t raw = vtu.rawValue;
        cr_assert((c == raw), "Failed for input value %d", (uint16_t)c);

        /// __then__ VtInputFromCharacters does not have data
        cr_assert(not(dut.canGetData()), "Failed for input value %d", (uint16_t)c);
    }
}

Test(VtInputFromCharacters, should_return_printable_characters_on_reading_an_octet_with_value_in_range_32_to_256_excluding_127) {
    /// __for__ any character _c_ in `range(32,256)` excluding 127
    for (uint16_t cint = 32; cint < 256; cint++) {
        if (cint == 127) continue;

        char8_t c = (char8_t)cint;
        cmspk::term::VtInputFromCharacters dut;

        /// __given__ VtInputFromCharacters has been reset
        dut.reset();

        /// __when__ VtInputFromCharacters is fed with _c_
        cr_assert(dut.append(c), "Failed for input value %d", (uint16_t)c);

        /// __then__ VtInputFromCharacters does not accept characters anymore
        cr_assert(not(dut.canAppend()), "Failed for input value %d", (uint16_t)c);

        /// __then__ VtInputFromCharacters does have data
        cr_assert(dut.canGetData(), "Failed for input value %d", (uint16_t)c);

        /// __then__ the VtInputFromCharacters will return a `std::variant` containing _c_.
        std::optional<cmspk::term::VtInput> vtin = dut.getData();
        cr_assert(vtin, "Failed for input value %d", (uint16_t)c);
        cr_assert(std::holds_alternative<char8_t>(*vtin), "Failed for input value %d", (uint16_t)c);
        char8_t printable = std::get<char8_t>(*vtin);
        cr_assert((c == printable), "Failed for input value %d", (uint16_t)c);

        /// __then__ VtInputFromCharacters does not have data
        cr_assert(not(dut.canGetData()), "Failed for input value %d", (uint16_t)c);
    }
}

struct ReadingKeySpec {
    char8_t givenChar;
    cmspk::term::VtInputKey expectedKey;
};
Test(VtInputFromCharacters, should_return_keys_on_reading_an_octet_with_value_127_or_in_range_0_to_27) {
    /// __for__ any character _c_ in [0,..,26,127]
    std::vector<ReadingKeySpec> toBeTested{
        {.givenChar = 0, .expectedKey = cmspk::term::VtInputKey::CTRL_SPACE}, {.givenChar = 1, .expectedKey = cmspk::term::VtInputKey::CTRL_A},
        {.givenChar = 2, .expectedKey = cmspk::term::VtInputKey::CTRL_B},     {.givenChar = 3, .expectedKey = cmspk::term::VtInputKey::CTRL_C},
        {.givenChar = 4, .expectedKey = cmspk::term::VtInputKey::CTRL_D},     {.givenChar = 5, .expectedKey = cmspk::term::VtInputKey::CTRL_E},
        {.givenChar = 6, .expectedKey = cmspk::term::VtInputKey::CTRL_F},     {.givenChar = 7, .expectedKey = cmspk::term::VtInputKey::CTRL_G},
        {.givenChar = 8, .expectedKey = cmspk::term::VtInputKey::CTRL_H},     {.givenChar = 9, .expectedKey = cmspk::term::VtInputKey::HTAB},
        {.givenChar = 10, .expectedKey = cmspk::term::VtInputKey::CTRL_J},    {.givenChar = 11, .expectedKey = cmspk::term::VtInputKey::CTRL_K},
        {.givenChar = 12, .expectedKey = cmspk::term::VtInputKey::CTRL_L},    {.givenChar = 13, .expectedKey = cmspk::term::VtInputKey::RETURN},
        {.givenChar = 14, .expectedKey = cmspk::term::VtInputKey::CTRL_N},    {.givenChar = 15, .expectedKey = cmspk::term::VtInputKey::CTRL_O},
        {.givenChar = 16, .expectedKey = cmspk::term::VtInputKey::CTRL_P},    {.givenChar = 17, .expectedKey = cmspk::term::VtInputKey::CTRL_Q},
        {.givenChar = 18, .expectedKey = cmspk::term::VtInputKey::CTRL_R},    {.givenChar = 19, .expectedKey = cmspk::term::VtInputKey::CTRL_S},
        {.givenChar = 20, .expectedKey = cmspk::term::VtInputKey::CTRL_T},    {.givenChar = 21, .expectedKey = cmspk::term::VtInputKey::CTRL_U},
        {.givenChar = 22, .expectedKey = cmspk::term::VtInputKey::CTRL_V},    {.givenChar = 23, .expectedKey = cmspk::term::VtInputKey::CTRL_W},
        {.givenChar = 24, .expectedKey = cmspk::term::VtInputKey::CTRL_X},    {.givenChar = 25, .expectedKey = cmspk::term::VtInputKey::CTRL_Y},
        {.givenChar = 26, .expectedKey = cmspk::term::VtInputKey::CTRL_Z},    {.givenChar = 127, .expectedKey = cmspk::term::VtInputKey::BACKSPACE}};
    for (ReadingKeySpec spec : toBeTested) {
        char8_t c = spec.givenChar;
        cmspk::term::VtInputFromCharacters dut;

        /// __given__ VtInputFromCharacters has been reset
        dut.reset();

        /// __when__ VtInputFromCharacters is fed with _c_
        cr_assert(dut.append(c), "Failed for input value %d", (uint16_t)c);

        /// __then__ VtInputFromCharacters does not accept characters anymore
        cr_assert(not(dut.canAppend()), "Failed for input value %d", (uint16_t)c);

        /// __then__ VtInputFromCharacters does have data
        cr_assert(dut.canGetData(), "Failed for input value %d", (uint16_t)c);

        /// __then__ the VtInputFromCharacters will return a `std::variant` containing the VtInputKey corresponding to _c_.
        std::optional<cmspk::term::VtInput> vtin = dut.getData();
        cr_assert(vtin, "Failed for input value %d", (uint16_t)c);
        cr_assert(std::holds_alternative<cmspk::term::VtInputKey>(*vtin), "Failed for input value %d", (uint16_t)c);
        cmspk::term::VtInputKey vtKey = std::get<cmspk::term::VtInputKey>(*vtin);
        cr_assert((spec.expectedKey == vtKey), "Failed for input value %d", (uint16_t)c);

        /// __then__ VtInputFromCharacters does not have data
        cr_assert(not(dut.canGetData()), "Failed for input value %d", (uint16_t)c);
    }
}

// ================[ END test suite ]==================
