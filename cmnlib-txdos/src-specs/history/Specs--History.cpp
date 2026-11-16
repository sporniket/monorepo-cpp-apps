// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 David SPORN
// ---
// This is part of **term library by sporniket**.
// A collection of utilities for writing terminal-hosted applications.
// ---
// @since v0.0.4

// standard libs
#include <expected>

// testing framework
#include <catch2/catch_test_macros.hpp>

// project
#include "cmspk/txdos/History.hpp"

// ================[ BEGIN common code ]==================
struct DummyRecord {
    uint8_t value = 0;
};
// ================[ END common code ]==================

// ================[ BEGIN specs suite ]==================

TEST_CASE("History", "[history]") {
    SECTION("Setting up", "How an history instantiation can behave") {
        GIVEN("An history initialized without arguments") {
            cmspk::txdos::History<DummyRecord> history;

            THEN("It is empty") {
                REQUIRE(history.size() == 0);
                REQUIRE(history.empty());
            }
            THEN("It has a default capacity of 100") { REQUIRE(history.capacity() == 100); }
        }
        GIVEN("An history initialized with a non-zero capacity") {
            cmspk::txdos::History<DummyRecord> history(50);

            THEN("It is empty") {
                REQUIRE(history.size() == 0);
                REQUIRE(history.empty());
            }
            THEN("It has the specified capacity") { REQUIRE(history.capacity() == 50); }
        }
        GIVEN("An history initialized with a zero capacity") {
            cmspk::txdos::History<DummyRecord> history(0);

            THEN("It is empty") {
                REQUIRE(history.size() == 0);
                REQUIRE(history.empty());
            }
            THEN("It has a forced capacity of 1") { REQUIRE(history.capacity() == 1); }
        }
    }
    SECTION("Storing and retrieving data") {
        GIVEN("An empty history") {
            cmspk::txdos::History<DummyRecord> history;

            THEN("It can store multiple records") {
                for (uint8_t i = 0; i < 10; i++) {
                    DummyRecord r{.value = i};
                    history.record(r);
                    REQUIRE(history.size() == (size_t)(i + 1));
                }
                AND_THEN("It can retrieves the last one") {
                    std::expected<const DummyRecord*, cmspk::txdos::History<DummyRecord>::Error> result = history.recall();
                    REQUIRE(result);
                    REQUIRE(result.value()->value == (uint8_t)9);
                }
            }
        }
        GIVEN("An history filled to its capacity") {
            cmspk::txdos::History<DummyRecord> history(5);
            for (uint8_t i = 0; i < 5; i++) {
                history.record(DummyRecord{.value = i});
            }
            REQUIRE(history.capacity() == (uint8_t)5);
            REQUIRE(history.size() == history.capacity());
            REQUIRE(history.recall());
            REQUIRE(history.recall().value()->value == (uint8_t)4);

            THEN("Storing a new record does not change its size") {
                history.record(DummyRecord{.value = (uint8_t)5});
                REQUIRE(history.capacity() == (uint8_t)5);
                REQUIRE(history.size() == history.capacity());
                REQUIRE(history.recall());
                REQUIRE(history.recall().value()->value == (uint8_t)5);

                AND_THEN("It has discarded the oldest record") {
                    for (uint8_t i = 0; i < 4; i++) history.goBack();  // return to oldest
                    REQUIRE(history.recall());
                    REQUIRE(history.recall().value()->value == (uint8_t)1);
                    history.goBack();
                    REQUIRE(!history.recall());
                    REQUIRE(history.recall().error().type == cmspk::txdos::History<DummyRecord>::ErrorType::out_of_range);
                }
            }
        }
    }
    SECTION("Navigating history") {
        GIVEN("An history loaded with 10 elements storing values from 0 to 9") {
            cmspk::txdos::History<DummyRecord> history;
            for (uint8_t i = 0; i < 10; i++) {
                DummyRecord r{.value = i};
                history.record(r);
            }

            THEN("It can't go forward") {
                REQUIRE(!history.canGoForward());
                std::expected<void, cmspk::txdos::History<DummyRecord>::Error> resultForward = history.goForward();
                REQUIRE(!resultForward);
                REQUIRE(resultForward.error().type == cmspk::txdos::History<DummyRecord>::ErrorType::out_of_range);
                std::expected<const DummyRecord*, cmspk::txdos::History<DummyRecord>::Error> resultRecallForward = history.recall();
                REQUIRE(resultRecallForward);
                REQUIRE(resultRecallForward.value()->value == (uint8_t)9);
            }

            THEN("It can go back") {
                REQUIRE(history.canGoBack());
                std::expected<void, cmspk::txdos::History<DummyRecord>::Error> resultBack = history.goBack();
                REQUIRE(resultBack);
                std::expected<const DummyRecord*, cmspk::txdos::History<DummyRecord>::Error> resultRecallBack = history.recall();
                REQUIRE(resultRecallBack);
                REQUIRE(resultRecallBack.value()->value == (uint8_t)8);

                AND_THEN("It can't go back further than before the oldest record") {
                    for (uint8_t i = 0; i < 8; i++) {
                        REQUIRE(history.canGoBack());
                        history.goBack();
                    }
                    REQUIRE(history.recall());
                    REQUIRE(history.recall().value()->value == (uint8_t)0);  // the oldest record
                    REQUIRE(history.canGoBack());
                    history.goBack();
                    REQUIRE(!history.recall());
                    REQUIRE(history.recall().error().type == cmspk::txdos::History<DummyRecord>::ErrorType::out_of_range);
                    REQUIRE(!history.canGoBack());
                }

                THEN("It can go forward") {
                    REQUIRE(history.canGoForward());
                    std::expected<void, cmspk::txdos::History<DummyRecord>::Error> resultForward = history.goForward();
                    REQUIRE(resultForward);
                    std::expected<const DummyRecord*, cmspk::txdos::History<DummyRecord>::Error> resultRecallForward = history.recall();
                    REQUIRE(resultRecallForward);
                    REQUIRE(resultRecallForward.value()->value == (uint8_t)9);
                }
            }

            WHEN("The cursor is in the middle of the history content") {
                for (uint8_t i = 0; i < 4; i++) {
                    REQUIRE(history.canGoBack());
                    history.goBack();
                }
                REQUIRE(history.recall());
                REQUIRE(history.recall().value()->value == (uint8_t)5);
                REQUIRE(history.size() == (size_t)10);
            }
        }
    }
}

// ================[ END specs suite ]==================
