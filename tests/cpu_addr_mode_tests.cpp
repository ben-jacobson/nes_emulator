#include "catch.hpp"
#include "test_fixtures.h"

TEST_CASE_METHOD(emulator_test_fixtures, "Dev environment, overflow and underflow function correctly", "[dev env]") {
    // our code base relies heavily on the uint8_t's and uint16_t's overflowing and underflowing as you'd expect. 
    // This test helps use sense check that this behaviour is working correctly with your development environment
    uint8_t overflower = 255;
    uint8_t result = test_cpu.debug_env_overflower_test(overflower, 5);
    CHECK(result == 4);

    overflower = 0;
    result = test_cpu.debug_env_overflower_test(overflower, -5);
    CHECK(result == 251);    
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - ABS", "[cpu instruction]") {
    hack_in_test_rom_data(0x8000 - PGM_ROM_ADDRESS_SPACE_START, 0xAA);
    hack_in_test_rom_data(0x8001 - PGM_ROM_ADDRESS_SPACE_START, 0xBB);

    test_cpu.set_program_counter(0x8000); 
    uint16_t program_counter_at_start = test_cpu.get_program_counter(); 
    CHECK(program_counter_at_start == 0x8000);

    test_cpu.addr_mode_ABS();   // we aren't calling cycle, so the address mode code will act on this instruction
    uint16_t last_fetched_address = test_cpu.get_last_fetched();
    REQUIRE(last_fetched_address == 0xBBAA);
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - ABSX", "[cpu instruction]") {
    // this address mode takes the effective address from PC and PC+1, then adds the X index to it. 
    hack_in_test_rom_data(0x8000 - PGM_ROM_ADDRESS_SPACE_START, 0x00);
    hack_in_test_rom_data(0x8001 - PGM_ROM_ADDRESS_SPACE_START, 0x02);
    test_cpu.debug_set_x_register(0x01);

    test_cpu.set_program_counter(0x8000); 
    uint16_t program_counter_at_start = test_cpu.get_program_counter(); 
    CHECK(program_counter_at_start == 0x8000);

    test_cpu.addr_mode_ABSX();   // we aren't calling cycle, so the address mode code will act on this instruction
    uint16_t last_fetched_address = test_cpu.get_last_fetched();
    REQUIRE(last_fetched_address == 0x0201);    
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - ABSY", "[cpu instruction]") {
    // this address mode takes the effective address from PC and PC+1, then adds the X index to it. 
    hack_in_test_rom_data(0x8000 - PGM_ROM_ADDRESS_SPACE_START, 0x00);
    hack_in_test_rom_data(0x8001 - PGM_ROM_ADDRESS_SPACE_START, 0x03);
    test_cpu.debug_set_y_register(0x02);

    test_cpu.set_program_counter(0x8000); 
    uint16_t program_counter_at_start = test_cpu.get_program_counter(); 
    CHECK(program_counter_at_start == 0x8000);

    test_cpu.addr_mode_ABSY();   // we aren't calling cycle, so the address mode code will act on this instruction
    uint16_t last_fetched_address = test_cpu.get_last_fetched();
    REQUIRE(last_fetched_address == 0x0302);   
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - ACCUM", "[cpu instruction]") {
    // instructions with the address mode act upon the accumulator. nothing to do except return 0
    REQUIRE(test_cpu.addr_mode_ACC() == 0);
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - IMM", "[cpu instruction]") {
    // in IMM address mode, the second byte of the instruction is the operand. 
    hack_in_test_rom_data(0x8000 - PGM_ROM_ADDRESS_SPACE_START, 0xAA);
    test_cpu.set_program_counter(0x8000);
    test_cpu.addr_mode_IMM();
    uint16_t fetched = test_cpu.get_last_fetched();
    REQUIRE(fetched == 0x8000);
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - INDX", "[cpu instruction]") {
    // Indexed Indirect Addressing - INDX
    hack_in_test_rom_data(0x8000 - PGM_ROM_ADDRESS_SPACE_START, 0x99);
    test_cpu.debug_set_x_register(0x05); 
    test_ram.write(0x009E, 0x50);   // 0x99 + 0x05 = 9E, the first address this address mode will look at
    test_ram.write(0x009F, 0xAA); 

    // sense check the RAM, we had issues with this
    test_bus.set_address(0x009E);
    CHECK(test_bus.read_data() == 0x50);
    test_bus.set_address(0x009F);
    CHECK(test_bus.read_data() == 0xAA);    

    // the second byte of the instruction is added to the contents of Index X, discarding the carry. 99 will be added to the x register (0x05) = 9E.
    test_cpu.addr_mode_INDX();
    // that zero page address contains the low order byte of the effective address and the next address location contains the high order byte
    uint16_t test_fetched_address = test_cpu.get_last_fetched();
    REQUIRE(test_fetched_address == 0xAA50);   
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - INDY", "[cpu instruction]") {
    // not to be confused with indirect addressing, this is indirect indexed and has a different order to indirect index X addressing
    hack_in_test_rom_data(0x8000 - PGM_ROM_ADDRESS_SPACE_START, 0x05); // a memory location in page zero
    test_ram.write(0x0005, 0x02); // a memory location in page zero
    test_cpu.debug_set_y_register(0x24); 

    // the operand is taken as a zero page address. The contents of this address is added to the contents of the Y register.
    test_cpu.set_program_counter(0x8000); // pretend that we just cycled into the operand
    test_cpu.addr_mode_INDY();

    uint16_t test_fetched_address = test_cpu.get_last_fetched();
    REQUIRE(test_fetched_address == 0x26);  // 0x24 + 2
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - INDI", "[cpu instruction]") {
    // Absolute Indirect works just like Absolute, except that it pulls data from that address to become the new program counter. Pretty much this is like a pointer

    // start by entering an address for the mode to read
    hack_in_test_rom_data(0x8000 - PGM_ROM_ADDRESS_SPACE_START, 0x10);  // the mode will first read this address and pull down 0x2211 as its address, then read EEFF as it's next address
    hack_in_test_rom_data(0x8001 - PGM_ROM_ADDRESS_SPACE_START, 0x80);
    hack_in_test_rom_data(0x8010 - PGM_ROM_ADDRESS_SPACE_START, 0xEE);
    hack_in_test_rom_data(0x8011 - PGM_ROM_ADDRESS_SPACE_START, 0xFF);    

    test_cpu.set_program_counter(0x8000); 
    uint16_t program_counter_at_start = test_cpu.get_program_counter(); 
    CHECK(program_counter_at_start == 0x8000);

    test_cpu.addr_mode_INDI();   // we aren't calling cycle, so the address mode code will act on this instruction
    uint16_t last_fetched_address = test_cpu.get_last_fetched();
    REQUIRE(last_fetched_address == 0xFFEE);    
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - IMP", "[cpu instruction]") {
    // implied mode does not mutate the instruction, so therefore will simply return 0 and do nothing else
    REQUIRE(test_cpu.addr_mode_IMP() == 0); // yet to be implemented
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - REL", "[cpu instruction]") {
    // relative addressing simply moved the program counter forward or backward, depending on the operand found in the second byte. 
    uint16_t pc_at_start = 0x8000;

    // test for incrementing PC
    uint8_t offset = 0b11111110; // twos complement for -2
    hack_in_test_rom_data(pc_at_start - PGM_ROM_ADDRESS_SPACE_START, offset);  // the mode will first read this address and pull down 0x2211 as its address, then read EEFF as it's next address
    test_cpu.set_program_counter(pc_at_start);  
    test_cpu.addr_mode_REL();
    uint16_t result = test_cpu.get_last_fetched();  
    CHECK(result == pc_at_start - 2 + 1); // program counter is incremented and takes into the account the PC during operand fetch

    // tests for decrementing PC
    offset = 0b00000011; // twos complement for +3
    hack_in_test_rom_data(pc_at_start - PGM_ROM_ADDRESS_SPACE_START, offset);  // the mode will first read this address and pull down 0x2211 as its address, then read EEFF as it's next address
    test_cpu.set_program_counter(pc_at_start);  
    test_cpu.addr_mode_REL();
    result = test_cpu.get_last_fetched();
    CHECK(result == pc_at_start + 3 + 1);   // program counter is incremented and takes into the account the PC during operand fetch  

    // tests for incrementing to the extreme case
    offset = 0b01111111; // twos complement for +127
    hack_in_test_rom_data(pc_at_start - PGM_ROM_ADDRESS_SPACE_START, offset);  // the mode will first read this address and pull down 0x2211 as its address, then read EEFF as it's next address
    test_cpu.set_program_counter(pc_at_start);  
    test_cpu.addr_mode_REL();
    result = test_cpu.get_last_fetched();
    CHECK(result == pc_at_start + 127 + 1);       

    // test for decrementing to the extreme case
    offset = 0b10000000; // twos complement for -128
    hack_in_test_rom_data(pc_at_start - PGM_ROM_ADDRESS_SPACE_START, offset);  // the mode will first read this address and pull down 0x2211 as its address, then read EEFF as it's next address
    test_cpu.set_program_counter(pc_at_start);  
    test_cpu.addr_mode_REL();
    result = test_cpu.get_last_fetched();
    REQUIRE(result == pc_at_start - 128 + 1);       
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - ZP", "[cpu instruction]") {
    // this address mode fetches only the second byte, and uses it as an address on page zero. 
    hack_in_test_rom_data(0x8000 - PGM_ROM_ADDRESS_SPACE_START, 0xBB);
    test_cpu.set_program_counter(0x8000);
    test_cpu.addr_mode_ZP();
    uint16_t address_fetched = test_cpu.get_last_fetched();
    REQUIRE(address_fetched == 0x00BB);
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - ZPX", "[cpu instruction]") {
    // this address mode works exactly the same as ZP, except it adds the value of the X register to the address.
    test_cpu.reset();
    hack_in_test_rom_data(0x8000 - PGM_ROM_ADDRESS_SPACE_START, 0xFC);

    // set the X register for a known state
    test_cpu.instr_INX(); // 1
    test_cpu.instr_INX(); // 2
    test_cpu.instr_INX(); // 3

    uint8_t index_x_result = test_cpu.get_x_index_reg_content();
    CHECK(index_x_result == 3);

    // Todo - do something that puts something into the X register. 
    test_cpu.set_program_counter(0x8000);
    test_cpu.addr_mode_ZPX();
    uint16_t address_fetched = test_cpu.get_last_fetched();
    REQUIRE(address_fetched == 0x00FF);

    // zero page addressing should never cross the page boundary
    test_cpu.instr_INX(); // 4
    test_cpu.set_program_counter(0x8000);
    test_cpu.addr_mode_ZPX();
    address_fetched = test_cpu.get_last_fetched();
    REQUIRE(address_fetched == 0x0000);
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu address mode - ZPY", "[cpu instruction]") {
    // this address mode works exactly the same as ZP, except it adds the value of the Y register to the address.
    test_cpu.reset();
    hack_in_test_rom_data(0x8000 - PGM_ROM_ADDRESS_SPACE_START, 0xFC);

    // set the Y register for a known state
    test_cpu.instr_INY(); // 1
    test_cpu.instr_INY(); // 2
    test_cpu.instr_INY(); // 3

    uint8_t index_y_result = test_cpu.get_y_index_reg_content();
    CHECK(index_y_result == 3);

    // Todo - do something that puts something into the X register. 
    test_cpu.set_program_counter(0x8000);
    test_cpu.addr_mode_ZPY();
    uint16_t address_fetched = test_cpu.get_last_fetched();
    REQUIRE(address_fetched == 0x00FF);

    // zero page addressing should never cross the page boundary
    test_cpu.instr_INY(); // 4
    test_cpu.set_program_counter(0x8000);
    test_cpu.addr_mode_ZPY();
    address_fetched = test_cpu.get_last_fetched();
    REQUIRE(address_fetched == 0x0000); // FF will have overflowed back to 00
}