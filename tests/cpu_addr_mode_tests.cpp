#include "catch.hpp"
#include "test_fixtures.h"

TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - ABS", "[cpu instruction]") {
    hack_in_test_rom_data(0x8000 - ROM_ADDRESS_SPACE_START, 0xEE);
    hack_in_test_rom_data(0x8001 - ROM_ADDRESS_SPACE_START, 0xDD);

    test_cpu.set_program_counter(0x8000); 
    uint16_t program_counter_at_start = test_cpu.get_program_counter(); 
    CHECK(program_counter_at_start == 0x8000);

    test_cpu.addr_mode_ABS();
    uint16_t last_fetched_address = test_cpu.get_last_fetched_address();
    REQUIRE(last_fetched_address == 0xDDEE);
}

/*TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - ABSX", "[cpu instruction]") {
    REQUIRE(0 != 0);
}*/

/*TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - ABSY", "[cpu instruction]") {
    REQUIRE(0 != 0);
}*/

TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - ACCUM", "[cpu instruction]") {
    REQUIRE(0 != 0);
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - IMM", "[cpu instruction]") {
    REQUIRE(0 != 0);
}

/*TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - INDI", "[cpu instruction]") {
    REQUIRE(0 != 0);
}*/

/*TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - INDX", "[cpu instruction]") {
    REQUIRE(0 != 0);
}*/

/*TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - INDY", "[cpu instruction]") {
    REQUIRE(0 != 0);
}*/

/*TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - IMP", "[cpu instruction]") {
    // implied mode does not mutate the instruction, so therefore will simply return 0 and do nothing else
    REQUIRE(test_cpu.addr_mode_IMP() == 0);
}*/

/*TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - REL", "[cpu instruction]") {
    REQUIRE(0 != 0);
}*/

TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - ZP", "[cpu instruction]") {
    REQUIRE(0 != 0);
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - ZPX", "[cpu instruction]") {
    REQUIRE(0 != 0);
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - ZPY", "[cpu instruction]") {
    REQUIRE(0 != 0);
}