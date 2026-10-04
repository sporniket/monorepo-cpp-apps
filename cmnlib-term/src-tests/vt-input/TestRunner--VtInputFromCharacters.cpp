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
        {.givenChar = 0, .expectedKey = cmspk::term::VtInputKey::ctrl_space}, {.givenChar = 1, .expectedKey = cmspk::term::VtInputKey::ctrl_a},
        {.givenChar = 2, .expectedKey = cmspk::term::VtInputKey::ctrl_b},     {.givenChar = 3, .expectedKey = cmspk::term::VtInputKey::ctrl_c},
        {.givenChar = 4, .expectedKey = cmspk::term::VtInputKey::ctrl_d},     {.givenChar = 5, .expectedKey = cmspk::term::VtInputKey::ctrl_e},
        {.givenChar = 6, .expectedKey = cmspk::term::VtInputKey::ctrl_f},     {.givenChar = 7, .expectedKey = cmspk::term::VtInputKey::ctrl_g},
        {.givenChar = 8, .expectedKey = cmspk::term::VtInputKey::ctrl_h},     {.givenChar = 9, .expectedKey = cmspk::term::VtInputKey::htab},
        {.givenChar = 10, .expectedKey = cmspk::term::VtInputKey::ctrl_j},    {.givenChar = 11, .expectedKey = cmspk::term::VtInputKey::ctrl_k},
        {.givenChar = 12, .expectedKey = cmspk::term::VtInputKey::ctrl_l},    {.givenChar = 13, .expectedKey = cmspk::term::VtInputKey::return_key},
        {.givenChar = 14, .expectedKey = cmspk::term::VtInputKey::ctrl_n},    {.givenChar = 15, .expectedKey = cmspk::term::VtInputKey::ctrl_o},
        {.givenChar = 16, .expectedKey = cmspk::term::VtInputKey::ctrl_p},    {.givenChar = 17, .expectedKey = cmspk::term::VtInputKey::ctrl_q},
        {.givenChar = 18, .expectedKey = cmspk::term::VtInputKey::ctrl_r},    {.givenChar = 19, .expectedKey = cmspk::term::VtInputKey::ctrl_s},
        {.givenChar = 20, .expectedKey = cmspk::term::VtInputKey::ctrl_t},    {.givenChar = 21, .expectedKey = cmspk::term::VtInputKey::ctrl_u},
        {.givenChar = 22, .expectedKey = cmspk::term::VtInputKey::ctrl_v},    {.givenChar = 23, .expectedKey = cmspk::term::VtInputKey::ctrl_w},
        {.givenChar = 24, .expectedKey = cmspk::term::VtInputKey::ctrl_x},    {.givenChar = 25, .expectedKey = cmspk::term::VtInputKey::ctrl_y},
        {.givenChar = 26, .expectedKey = cmspk::term::VtInputKey::ctrl_z},    {.givenChar = 127, .expectedKey = cmspk::term::VtInputKey::backspace}};
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

struct ReadingMultiCharKeySpec {
    std::string description;
    std::basic_string<char8_t> givenInput;
    cmspk::term::VtInputKey expectedKey;
};
Test(VtInputFromCharacters, should_return_keys_on_recognizing_a_multiple_characters_sequence) {
    /// __For any enum value _K_ in `VtInputKey` that is matched by a sequence of at least 2 characters__
    std::vector<ReadingMultiCharKeySpec> toBeTested{
        {.description = "Arrow up", .givenInput = u8"\x1b[A", .expectedKey = cmspk::term::VtInputKey::arrow_up},
        {.description = "Arrow down", .givenInput = u8"\x1b[B", .expectedKey = cmspk::term::VtInputKey::arrow_down},
        {.description = "Arrow left", .givenInput = u8"\x1b[C", .expectedKey = cmspk::term::VtInputKey::arrow_left},
        {.description = "Arrow right", .givenInput = u8"\x1b[D", .expectedKey = cmspk::term::VtInputKey::arrow_right}};
    for (ReadingMultiCharKeySpec spec : toBeTested) {
        cmspk::term::VtInputFromCharacters dut;

        /// __given__ VtInputFromCharacters has been reset
        dut.reset();

        /// __when__ VtInputFromCharacters is fed with a character sequence that should be recognized as _K_
        for (char8_t c : spec.givenInput) {
            cr_assert(dut.canAppend(), "Failed for input value from %s, before appending char code %d", spec.description.c_str(), (uint16_t)c);
            cr_assert(not(dut.canGetData()), "Failed for input value from %s, before appending char code %d", spec.description.c_str(), (uint16_t)c);
            cr_assert(dut.append(c), "Failed for input value from %s, when appending char code %d", spec.description.c_str(), (uint16_t)c);
        }

        /// __then__ VtInputFromCharacters does not accept characters anymore
        cr_assert(not(dut.canAppend()), "Failed for input value from %s", spec.description.c_str());

        /// __then__ VtInputFromCharacters does have data
        cr_assert(dut.canGetData(), "Failed for input value from %s", spec.description.c_str());

        /// __then__ the VtInputFromCharacters will return a `std::variant` containing _K_.
        std::optional<cmspk::term::VtInput> vtin = dut.getData();
        cr_assert(vtin, "Failed for input value from %s", spec.description.c_str());
        cr_assert(std::holds_alternative<cmspk::term::VtInputKey>(*vtin), "Failed for input value from %s", spec.description.c_str());
        cmspk::term::VtInputKey vtKey = std::get<cmspk::term::VtInputKey>(*vtin);
        cr_assert((spec.expectedKey == vtKey), "Failed for input value from %s, expected %d, got %d", spec.description.c_str(), spec.expectedKey, vtKey);

        /// __then__ VtInputFromCharacters does not have data
        cr_assert(not(dut.canGetData()), "Failed for input value from %s", spec.description.c_str());
    }
}

Test(VtInputFromCharacters, should_return_vt_report_on_recognizing_cursor_position_report) {
    /// __given__ VtInputFromCharacters has been reset
    cmspk::term::VtInputFromCharacters dut;
    dut.reset();

    /// __when__ VtInputFromCharacters is fed with the character sequence "\x1b[24;80R"
    std::basic_string<char8_t> toBeTested = u8"\x1b[24;80R";
    for (char8_t c : toBeTested) {
        cr_assert(dut.canAppend(), "Failed before appending char code %d", (uint16_t)c);
        cr_assert(not(dut.canGetData()), "Failed before appending char code %d", (uint16_t)c);
        cr_assert(dut.append(c), "Failed when appending char code %d", (uint16_t)c);
    }

    /// __then__ VtInputFromCharacters does not accept characters anymore
    cr_assert(not(dut.canAppend()), "Failed to match report");

    /// __then__ VtInputFromCharacters does have data
    cr_assert(dut.canGetData(), "Failed to match report");

    /// __then__ the VtInputFromCharacters will return a `std::variant` containing a `std::variant` of type `VtInputCursorPositionReport` row 24 and col 80.
    std::optional<cmspk::term::VtInput> vtin = dut.getData();
    cr_assert(vtin, "Failed to match report");
    cr_assert(std::holds_alternative<cmspk::term::VtInputReport>(*vtin), "Failed to match report");
    cmspk::term::VtInputReport vtReport = std::get<cmspk::term::VtInputReport>(*vtin);
    cr_assert(std::holds_alternative<cmspk::term::VtInputCursorPositionReport>(vtReport), "Failed to match report");
    cmspk::term::VtInputCursorPositionReport vtCursorReport = std::get<cmspk::term::VtInputCursorPositionReport>(vtReport);
    cr_assert(vtCursorReport.col == 80, "Expected col to be 80, got %d", vtCursorReport.col);
    cr_assert(vtCursorReport.row == 24, "Expected row to be 24, got %d", vtCursorReport.row);

    /// __then__ VtInputFromCharacters does not have data
    cr_assert(not(dut.canGetData()), "Failed to match report");
}

Test(VtInputFromCharacters, should_fall_back_to_single_character_conversion_when_a_sequence_is_finally_not_recognized) {
    /// __given__ VtInputFromCharacters has been reset
    cmspk::term::VtInputFromCharacters dut;
    dut.reset();

    /// __when__ VtInputFromCharacters is fed with the character sequence "\x1bA"
    std::basic_string<char8_t> toBeTested = u8"\x1b";
    toBeTested.append(u8"A");
    for (char8_t c : toBeTested) {
        cr_assert(dut.canAppend(), "Failed before appending char code %d", (uint16_t)c);
        cr_assert(not(dut.canGetData()), "Failed before appending char code %d", (uint16_t)c);
        cr_assert(dut.append(c), "Failed when appending char code %d", (uint16_t)c);
    }

    /// __then__ VtInputFromCharacters does not accept characters anymore
    cr_assert(not(dut.canAppend()));

    /// __then__ VtInputFromCharacters does have data
    cr_assert(dut.canGetData());

    /// __then__ the VtInputFromCharacters will return a `std::variant` containing the `VtInputKey` value `escape`
    std::optional<cmspk::term::VtInput> vtin = dut.getData();
    cr_assert(vtin);
    cr_assert(std::holds_alternative<cmspk::term::VtInputKey>(*vtin));
    cmspk::term::VtInputKey vtKey = std::get<cmspk::term::VtInputKey>(*vtin);
    cr_assert((cmspk::term::VtInputKey::escape == vtKey), "Expected %d, got %d", cmspk::term::VtInputKey::escape, vtKey);

    /// __then__ VtInputFromCharacters does have data
    cr_assert(dut.canGetData());

    /// __then__ the VtInputFromCharacters will return a `std::variant` containing the printable character `A`
    vtin = dut.getData();
    cr_assert(vtin);
    cr_assert(std::holds_alternative<char8_t>(*vtin));
    char8_t printable = std::get<char8_t>(*vtin);
    cr_assert((65 == printable), "Expected %d, got %d", 65, (uint16_t)printable);

    /// __then__ VtInputFromCharacters does not have data
    cr_assert(not(dut.canGetData()));
}

Test(VtInputFromCharacters, should_start_a_new_sequence_match_when_the_current_sequence_is_broken_by_the_next_character) {
    /// __given__ VtInputFromCharacters has been reset
    cmspk::term::VtInputFromCharacters dut;
    dut.reset();

    /// __when__ VtInputFromCharacters is fed with the character sequence "\x1b[\x1b[A"
    std::basic_string<char8_t> toBeTested = u8"\x1b";
    toBeTested.append(u8"[\x1b");
    toBeTested.append(u8"[A");
    uint16_t count = 0;
    for (char8_t c : toBeTested) {
        ++count;
        cr_assert(dut.canAppend(), "Failed before appending char code %d (position %d)", (uint16_t)c, count);
        if (count < 4) {
            cr_assert(not(dut.canGetData()), "Failed before appending char code %d (position %d)", (uint16_t)c, count);
        } else {
            cr_assert(dut.canGetData(), "Failed before appending char code %d (position %d)", (uint16_t)c, count);
        }
        cr_assert(dut.append(c), "Failed when appending char code %d (position %d)", (uint16_t)c, count);
    }

    /// __then__ VtInputFromCharacters does not accept characters anymore
    cr_assert(not(dut.canAppend()));

    /// __then__ VtInputFromCharacters does have data
    cr_assert(dut.canGetData());

    /// __then__ the VtInputFromCharacters will return a `std::variant` containing the `VtInputKey` value `escape`
    std::optional<cmspk::term::VtInput> vtin = dut.getData();
    cr_assert(vtin);
    cr_assert(std::holds_alternative<cmspk::term::VtInputKey>(*vtin));
    cmspk::term::VtInputKey vtKey = std::get<cmspk::term::VtInputKey>(*vtin);
    cr_assert((cmspk::term::VtInputKey::escape == vtKey), "Expected %d, got %d", cmspk::term::VtInputKey::escape, vtKey);

    /// __then__ VtInputFromCharacters does have data
    cr_assert(dut.canGetData());

    /// __then__ the VtInputFromCharacters will return a `std::variant` containing the printable character `[` ;
    vtin = dut.getData();
    cr_assert(vtin);
    cr_assert(std::holds_alternative<char8_t>(*vtin));
    char8_t printable = std::get<char8_t>(*vtin);
    cr_assert((91 == printable), "Expected %d, got %d", 91, (uint16_t)printable);

    /// __then__ VtInputFromCharacters does have data
    cr_assert(dut.canGetData());

    /// __then__ the VtInputFromCharacters will return a `std::variant` containing the `VtInputKey` value `ARROW_UP` ;
    vtin = dut.getData();
    cr_assert(vtin);
    cr_assert(std::holds_alternative<cmspk::term::VtInputKey>(*vtin));
    vtKey = std::get<cmspk::term::VtInputKey>(*vtin);
    cr_assert((cmspk::term::VtInputKey::arrow_up == vtKey), "Expected %d, got %d", cmspk::term::VtInputKey::arrow_up, vtKey);

    /// __then__ VtInputFromCharacters does not have data
    cr_assert(not(dut.canGetData()));
}

Test(VtInputFromCharacters, should_give_access_to_available_data_from_the_broken_previous_sequence_while_still_accepting_characters_for_the_current_sequence) {
    /// __given__ VtInputFromCharacters has been reset
    cmspk::term::VtInputFromCharacters dut;
    dut.reset();

    /// __when__ VtInputFromCharacters is fed with the character sequence "\x1b[\x1b"
    std::basic_string<char8_t> toBeTested = u8"\x1b";
    toBeTested.append(u8"[\x1b");
    uint16_t count = 0;
    for (char8_t c : toBeTested) {
        ++count;
        cr_assert(dut.canAppend(), "Failed before appending char code %d (position %d)", (uint16_t)c, count);
        if (count < 4) {
            cr_assert(not(dut.canGetData()), "Failed before appending char code %d (position %d)", (uint16_t)c, count);
        } else {
            cr_assert(dut.canGetData(), "Failed before appending char code %d (position %d)", (uint16_t)c, count);
        }
        cr_assert(dut.append(c), "Failed when appending char code %d (position %d)", (uint16_t)c, count);
    }

    /// __then__ VtInputFromCharacters still accept characters
    cr_assert(dut.canAppend());

    /// __then__ VtInputFromCharacters does have data
    cr_assert(dut.canGetData());

    /// __then__ the VtInputFromCharacters will return a `std::variant` containing the `VtInputKey` value `escape`
    std::optional<cmspk::term::VtInput> vtin = dut.getData();
    cr_assert(vtin);
    cr_assert(std::holds_alternative<cmspk::term::VtInputKey>(*vtin));
    cmspk::term::VtInputKey vtKey = std::get<cmspk::term::VtInputKey>(*vtin);
    cr_assert((cmspk::term::VtInputKey::escape == vtKey), "Expected %d, got %d", cmspk::term::VtInputKey::escape, vtKey);

    /// __then__ VtInputFromCharacters does have data
    cr_assert(dut.canGetData());

    /// __then__ the VtInputFromCharacters will return  a `std::variant` containing the printable character `[`
    vtin = dut.getData();
    cr_assert(vtin);
    cr_assert(std::holds_alternative<char8_t>(*vtin));
    char8_t printable = std::get<char8_t>(*vtin);
    cr_assert((91 == printable), "Expected %d, got %d", 91, (uint16_t)printable);

    /// __then__ VtInputFromCharacters does not have data
    cr_assert(not(dut.canGetData()));
}

Test(VtInputFromCharacters, should_fall_back_to_single_character_conversion_when_it_is_aborted_in_the_middle_of_a_multi_octets_sequence) {
    /// __given__ VtInputFromCharacters has been reset and been fed with the character sequence "\x1b["
    cmspk::term::VtInputFromCharacters dut;
    dut.reset();
    std::basic_string<char8_t> toBeTested = u8"\x1b";
    toBeTested.append(u8"[");
    for (char8_t c : toBeTested) {
        cr_assert(dut.canAppend(), "Failed before appending char code %d", (uint16_t)c);
        cr_assert(not(dut.canGetData()), "Failed before appending char code %d", (uint16_t)c);
        cr_assert(dut.append(c), "Failed when appending char code %d", (uint16_t)c);
    }

    /// __when__ VtInputFromCharacters is aborted
    dut.abort();

    /// __then__ VtInputFromCharacters does not accept characters anymore
    cr_assert(not(dut.canAppend()));

    /// __then__ VtInputFromCharacters does have data
    cr_assert(dut.canGetData());

    /// __then__ the VtInputFromCharacters will return a `std::variant` containing the `VtInputKey` value `escape` ;
    std::optional<cmspk::term::VtInput> vtin = dut.getData();
    cr_assert(vtin);
    cr_assert(std::holds_alternative<cmspk::term::VtInputKey>(*vtin));
    cmspk::term::VtInputKey vtKey = std::get<cmspk::term::VtInputKey>(*vtin);
    cr_assert((cmspk::term::VtInputKey::escape == vtKey), "Expected %d, got %d", cmspk::term::VtInputKey::escape, vtKey);

    /// __then__ VtInputFromCharacters does have data
    cr_assert(dut.canGetData());

    /// __then__ the VtInputFromCharacters will return a `std::variant` containing the printable character `[` ;
    vtin = dut.getData();
    cr_assert(vtin);
    cr_assert(std::holds_alternative<char8_t>(*vtin));
    char8_t printable = std::get<char8_t>(*vtin);
    cr_assert((91 == printable), "Expected %d, got %d", 91, (uint16_t)printable);

    /// __then__ VtInputFromCharacters does not have data
    cr_assert(not(dut.canGetData()));
}

Test(VtInputFromCharacters, should_clear_its_internal_state_when_it_is_reset) {
    /// __given__ VtInputFromCharacters has been reset and been fed with the character sequence "\x1b["
    cmspk::term::VtInputFromCharacters dut;
    dut.reset();
    std::basic_string<char8_t> toBeTested = u8"\x1b";
    toBeTested.append(u8"[");
    for (char8_t c : toBeTested) {
        cr_assert(dut.canAppend(), "Failed before appending char code %d", (uint16_t)c);
        cr_assert(not(dut.canGetData()), "Failed before appending char code %d", (uint16_t)c);
        cr_assert(dut.append(c), "Failed when appending char code %d", (uint16_t)c);
    }

    /// __when__ VtInputFromCharacters is reset
    dut.reset();

    /// __then__ VtInputFromCharacters still accept characters
    cr_assert(dut.canAppend());

    /// __then__ the VtInputFromCharacters contains no data
    cr_assert(not(dut.canGetData()));

    /// __when__ VtInputFromCharacters is fed with the single character `A`
    dut.append((char8_t)65);

    /// __then__ VtInputFromCharacters does not accept characters anymore
    cr_assert(not(dut.canAppend()));

    /// __then__ the VtInputFromCharacters will return a `std::variant` containing `A`.
    cr_assert(dut.canGetData());
    std::optional<cmspk::term::VtInput> vtin = dut.getData();
    cr_assert(vtin);
    cr_assert(std::holds_alternative<char8_t>(*vtin));
    char8_t printable = std::get<char8_t>(*vtin);
    cr_assert((65 == printable), "Expected %d, got %d", 65, (uint16_t)printable);

    /// __then__ VtInputFromCharacters does not have data
    cr_assert(not(dut.canGetData()));
}
// ================[ END test suite ]==================
