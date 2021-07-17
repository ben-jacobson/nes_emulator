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
    REQUIRE(test_cpu.addr_mode_ACCUM() == 0);
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - IMM", "[cpu instruction]") {
    uint16_t program_counter_before_address_mode = test_cpu.get_program_counter();
    uint8_t result = test_cpu.addr_mode_IMM(); // program counter will tick one forward
    uint16_t last_fetched_address = test_cpu.get_last_fetched_address(); 
    // the last fetched data should now be set to the program counter once clock cycle ago
    CHECK(last_fetched_address == program_counter_before_address_mode); 
    REQUIRE(result == 0);
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