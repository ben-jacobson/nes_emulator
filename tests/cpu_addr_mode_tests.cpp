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
    test_cpu.reset();
    hack_in_test_rom_data(0x8000 - ROM_ADDRESS_SPACE_START, 0xFC);

    // set the X register for a known state
    test_cpu.instr_INX(); // 1
    test_cpu.instr_INX(); // 2
    test_cpu.instr_INX(); // 3

    uint8_t index_x_result = test_cpu.get_x_index_reg_content();
    CHECK(index_x_result == 3);

    // Todo - do something that puts something into the X register. 
    test_cpu.set_program_counter(0x8000);
    test_cpu.addr_mode_ZPX();
    uint16_t address_fetched = test_cpu.get_last_fetched_address();
    REQUIRE(address_fetched == 0x00FF);

    // zero page addressing should never cross the page boundary
    test_cpu.instr_INX(); // 4
    test_cpu.set_program_counter(0x8000);
    test_cpu.addr_mode_ZPX();
    address_fetched = test_cpu.get_last_fetched_address();
    REQUIRE(address_fetched == 0x0000);
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - ZPY", "[cpu instruction]") {
    // this address mode works exactly the same as ZP, except it adds the value of the Y register to the address.
    test_cpu.reset();
    hack_in_test_rom_data(0x8000 - ROM_ADDRESS_SPACE_START, 0xFC);

    // set the Y register for a known state
    test_cpu.instr_INY(); // 1
    test_cpu.instr_INY(); // 2
    test_cpu.instr_INY(); // 3

    uint8_t index_y_result = test_cpu.get_y_index_reg_content();
    CHECK(index_y_result == 3);

    // Todo - do something that puts something into the X register. 
    test_cpu.set_program_counter(0x8000);
    test_cpu.addr_mode_ZPY();
    uint16_t address_fetched = test_cpu.get_last_fetched_address();
    REQUIRE(address_fetched == 0x00FF);

    // zero page addressing should never cross the page boundary
    test_cpu.instr_INY(); // 4
    test_cpu.set_program_counter(0x8000);
    test_cpu.addr_mode_ZPY();
    address_fetched = test_cpu.get_last_fetched_address();
    REQUIRE(address_fetched == 0x0000); // FF will have overflowed back to 00
}