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
    REQUIRE(0 != 0); // yet to be implemented
}*/

/*TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - ABSY", "[cpu instruction]") {
    REQUIRE(0 != 0); // yet to be implemented
}*/

TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - ACCUM", "[cpu instruction]") {
    // instructions with the address mode act upon the accumulator. nothing to do except return 0
    REQUIRE(test_cpu.addr_mode_ACCUM() == 0);
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - IMM", "[cpu instruction]") {
    // in IMM address mode, the second byte of the instruction is the operand. 
    hack_in_test_rom_data(0x8000 - ROM_ADDRESS_SPACE_START, 0xAA);
    test_cpu.set_program_counter(0x8000);
    test_cpu.addr_mode_IMM();
    uint8_t fetched_operand = test_cpu.get_last_fetched_operand();
    REQUIRE(fetched_operand == 0xAA);
}

/*TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - INDI", "[cpu instruction]") {
    REQUIRE(0 != 0); // yet to be implemented
}*/

/*TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - INDX", "[cpu instruction]") {
    REQUIRE(0 != 0); // yet to be implemented
}*/

/*TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - INDY", "[cpu instruction]") {
    REQUIRE(0 != 0); // yet to be implemented
}*/

/*TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - IMP", "[cpu instruction]") {
    // implied mode does not mutate the instruction, so therefore will simply return 0 and do nothing else
    REQUIRE(test_cpu.addr_mode_IMP() == 0); // yet to be implemented
}*/

/*TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - REL", "[cpu instruction]") {
    REQUIRE(0 != 0); // yet to be implemented
}*/

TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - ZP", "[cpu instruction]") {
    // this address mode fetches only the second byte, and uses it as an address on page zero. 
    hack_in_test_rom_data(0x8000 - ROM_ADDRESS_SPACE_START, 0xBB);
    test_cpu.set_program_counter(0x8000);
    test_cpu.addr_mode_ZP();
    uint16_t address_fetched = test_cpu.get_last_fetched_address();
    REQUIRE(address_fetched == 0x00BB);
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - ZPX", "[cpu instruction]") {
    // this address mode works exactly the same as ZP, except it adds the value of the X register to the address.
    hack_in_test_rom_data(0x8000 - ROM_ADDRESS_SPACE_START, 0x99);
    // Todo - do something that puts something into the X register. 
    REQUIRE(0 != 0);
    test_cpu.set_program_counter(0x8000);
    test_cpu.addr_mode_ZP();
    uint16_t address_fetched = test_cpu.get_last_fetched_address();
    REQUIRE(address_fetched == 0x0099);    
    REQUIRE(0 != 0);
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - ZPY", "[cpu instruction]") {
    // this address mode works exactly the same as ZP, except it adds the value of the Y register to the address.
    hack_in_test_rom_data(0x8000 - ROM_ADDRESS_SPACE_START, 0x99);
    // Todo - do something that puts something into the T register. 
    REQUIRE(0 != 0);
    test_cpu.set_program_counter(0x8000);
    test_cpu.addr_mode_ZP();
    uint16_t address_fetched = test_cpu.get_last_fetched_address();
    REQUIRE(address_fetched == 0x0099);        
    REQUIRE(0 != 0);
}