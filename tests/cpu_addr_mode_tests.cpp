#include "catch.hpp"
#include "test_fixtures.h"

TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - ABS", "[cpu instruction]") {
    // cannot be tested until we load some instructions in and execute them.
    REQUIRE(0 != 0);
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - ABSX", "[cpu instruction]") {
    REQUIRE(0 != 0);
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - ABSY", "[cpu instruction]") {
    REQUIRE(0 != 0);
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - ACCUM", "[cpu instruction]") {
    REQUIRE(0 != 0);
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - IMM", "[cpu instruction]") {
    REQUIRE(0 != 0);
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - INDI", "[cpu instruction]") {
    REQUIRE(0 != 0);
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - INDX", "[cpu instruction]") {
    REQUIRE(0 != 0);
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - INDY", "[cpu instruction]") {
    REQUIRE(0 != 0);
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - IMP", "[cpu instruction]") {
    // implied mode does not mutate the instruction, so therefore will simply return 0 and do nothing else
    REQUIRE(test_cpu.addr_mode_IMP() == 0);
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - REL", "[cpu instruction]") {
    REQUIRE(0 != 0);
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - ZP", "[cpu instruction]") {
    REQUIRE(0 != 0);
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - ZPX", "[cpu instruction]") {
    REQUIRE(0 != 0);
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - ZPY", "[cpu instruction]") {
    REQUIRE(0 != 0);
}