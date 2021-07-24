#include "catch.hpp"
#include "test_fixtures.h"
    

TEST_CASE("cpu - test 16bit splitting logic") {
    // we use this logic in a lot of private methods, since we can't test it, we'll just test our pattern in isolation instead. 

    uint16_t test_address = 0xABCD;
    uint8_t test_address_high = ((test_address & 0xFF00) >> 8);
    uint8_t test_address_low = (test_address & 0x00FF);

    REQUIRE(test_address_high == 0xAB);
    REQUIRE(test_address_low == 0xCD);
}
TEST_CASE_METHOD(emulator_test_fixtures, "cpu - Test get_status_flags") {
    test_cpu.reset(); // after resetting the cpu, the IRQ bit should be 1 (disabled)
    // test that the interrupt disable bit was set (disabled)
    REQUIRE(test_cpu.get_status_flags_struct().i == 1);
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu - Test get_status_flags_struct") {
    test_cpu.reset(); // after resetting the cpu, the IRQ bit should be 1 (disabled)
    // test that the interrupt disable bit was set (disabled)
    uint8_t result = test_cpu.get_status_flags() & (1 << IRQ_FLAG);
    CHECK(result == (1 << IRQ_FLAG));
}


TEST_CASE_METHOD(emulator_test_fixtures, "cpu - Test cycle", "[cpu]") {
    hack_in_test_rom_data(RESET_VECTOR_LOW - PGM_ROM_ADDRESS_SPACE_START, 0xDD);        // set reset vector so that CPU knows where to go to look for start of program
    hack_in_test_rom_data(RESET_VECTOR_HIGH - PGM_ROM_ADDRESS_SPACE_START, 0xEE);
    hack_in_test_rom_data(0xEEDD - PGM_ROM_ADDRESS_SPACE_START, 0xEA); // a nop instruction, do nothing
    hack_in_test_rom_data(0xEEDE - PGM_ROM_ADDRESS_SPACE_START, 0xE8); // an inx instruction, increment X index
    hack_in_test_rom_data(0xEEDF - PGM_ROM_ADDRESS_SPACE_START, 0x08); // a PHP instruction, push program status to stack
    test_cpu.reset(); // program will start at address EEDD

    test_bus.set_address(0xEEDD);
    uint8_t rom_sense_check = test_bus.read_data();
    CHECK (rom_sense_check == 0xEA);

    uint16_t program_counter = test_cpu.get_program_counter(); 
    CHECK(program_counter == 0xEEDD);

    // the cpu takes 6 clock cycles to commence
    for (uint8_t i = 0; i < 6; i++) {
        test_cpu.cycle();
    }    

    CHECK (test_cpu.debug_get_cycle_count() == 6);
    test_cpu.cycle();
    uint8_t last_fetched_opcode = test_cpu.get_last_fetched_opcode(); 
    CHECK(last_fetched_opcode == 0xEA);    
    test_cpu.cycle(); // NOP takes two clock cycles, so will require two pulses to get to next instruction
    CHECK (test_cpu.debug_get_cycle_count() == 8);

    program_counter = test_cpu.get_program_counter(); 
    CHECK(program_counter == 0xEEDE);     // inx instruction at this point
    test_cpu.cycle();
    last_fetched_opcode = test_cpu.get_last_fetched_opcode(); 
    CHECK(last_fetched_opcode == 0xE8);
    uint8_t x_index_contents = test_cpu.get_x_index_reg_content();
    CHECK(x_index_contents == 1);
    test_cpu.cycle(); // INX takes two clock cycles
    CHECK (test_cpu.debug_get_cycle_count() == 10);

    program_counter = test_cpu.get_program_counter(); 
    CHECK(program_counter == 0xEEDF); // PHP instruction  
    uint8_t stack_pointer = test_cpu.get_stack_pointer();
    test_cpu.cycle();       
    last_fetched_opcode = test_cpu.get_last_fetched_opcode(); 
    CHECK(last_fetched_opcode == 0x08);
    uint8_t status_flags = test_cpu.get_status_flags();
    test_bus.set_address(STACK_START + stack_pointer);
    uint8_t stack_contents = test_bus.read_data();
    CHECK(stack_contents == status_flags);
    test_cpu.cycle();
    test_cpu.cycle();   // PHP takes 3 instruction cycles
    CHECK (test_cpu.debug_get_cycle_count() == 13);

    program_counter = test_cpu.get_program_counter(); 
    CHECK(program_counter == 0xEEE0);         
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu - Test reset", "[cpu]") {
    // put some data into the reset vector, which is a section of ROM memory
    hack_in_test_rom_data(RESET_VECTOR_LOW - PGM_ROM_ADDRESS_SPACE_START, 0xDD);
    hack_in_test_rom_data(RESET_VECTOR_HIGH - PGM_ROM_ADDRESS_SPACE_START, 0xEE);

    uint8_t reset_vector_low = test_cart.read_rom(RESET_VECTOR_LOW);
    CHECK(reset_vector_low == 0xDD); // check that it landed properly in ROM

    uint8_t rom_test_result = test_cart.read(RESET_VECTOR_HIGH);
    CHECK(rom_test_result == 0xEE);

    test_bus.set_address(RESET_VECTOR_HIGH);
    uint8_t reset_vector_on_bus = test_bus.read_data();
    CHECK(reset_vector_on_bus == 0xEE); // check that it landed properly in all of the cart address space.     

    test_cpu.reset(); 
    uint16_t result_program_counter = test_cpu.get_program_counter();   
    // resetting the CPU will set the program counter to the reset vector.
    CHECK(result_program_counter == 0xEEDD); // check that this copied into program counter

    // unused status flag should be 1
    CHECK(test_cpu.get_status_flags_struct().u == 1);
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu - Test IRQ", "[cpu]") { 
    test_cpu.set_program_counter(0xAADD);    // emulate being at a random address
    test_cpu.instr_CLI(); // cpu defaults to IRQ disabled, clear this bit to enable IRQ

    uint16_t stack_pointer_at_start = STACK_START + test_cpu.get_stack_pointer();
    uint16_t program_counter_at_start = test_cpu.get_program_counter();  
    uint8_t program_counter_at_start_low = (program_counter_at_start & 0x00FF);
    uint8_t program_counter_at_start_high = ((program_counter_at_start & 0xFF00) >> 8);     

    hack_in_test_rom_data(0xFFFF - PGM_ROM_ADDRESS_SPACE_START, 0xBB); // put something in the IRQ vector
    hack_in_test_rom_data(0xFFFE - PGM_ROM_ADDRESS_SPACE_START, 0xAA);

    CHECK(test_cpu.get_status_flags_struct().i == 0); // is the interrupt enabled (0)?

    bool result = test_cpu.IRQ();   // call the IRQ and see if returned true. 
    CHECK(result == true);
    
    // program counter should now be on the stack
    uint8_t result_pgm_counter_high = test_ram.read(stack_pointer_at_start);  // the high 8 bits of pgm counter were loaded in first
    uint8_t result_pgm_counter_low = test_ram.read(stack_pointer_at_start - 1); // then the low 8 bits of pgm counter
    CHECK(result_pgm_counter_high == program_counter_at_start_high);
    CHECK(result_pgm_counter_low == program_counter_at_start_low);
        
    // also status registers
    uint8_t result_stack_pointer_status_regs = test_ram.read(stack_pointer_at_start - 2);
    CHECK(result_stack_pointer_status_regs == test_cpu.get_status_flags());

    REQUIRE(test_cpu.get_program_counter() == 0xBBAA); // was the program counter set to the IRQ vector?
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu - Test NMI", "[cpu]") {
    uint16_t stack_pointer_at_start = STACK_START + test_cpu.get_stack_pointer();
    uint16_t program_counter_at_start = test_cpu.get_program_counter();  
    uint8_t program_counter_at_start_low = (program_counter_at_start & 0xFF);
    uint8_t program_counter_at_start_high = ((program_counter_at_start & 0xFF00) >> 8); 

    hack_in_test_rom_data(0xFFFB - PGM_ROM_ADDRESS_SPACE_START, 0xEE);  // put something in the NMI vector
    hack_in_test_rom_data(0xFFFA - PGM_ROM_ADDRESS_SPACE_START, 0xFF); 

    bool result = test_cpu.NMI();   
    CHECK(result == true);

    // program counter should now be on the stack
    uint8_t result_pgm_counter_high = test_ram.read(stack_pointer_at_start); 
    uint8_t result_pgm_counter_low = test_ram.read(stack_pointer_at_start - 1);
    CHECK(result_pgm_counter_high == program_counter_at_start_high);
    CHECK(result_pgm_counter_low == program_counter_at_start_low);
        
    // also status registers
    uint8_t result_stack_pointer_status_regs = test_ram.read(stack_pointer_at_start - 2);
    CHECK(result_stack_pointer_status_regs == test_cpu.get_status_flags());

    REQUIRE(test_cpu.get_program_counter() == 0xEEFF); // was the program counter set to the NMI vector?
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu - Test set and get stack pointer") {
    // first attempt to set the stack out of bounds
    test_cpu.set_stack_pointer(0xFFFF); 
    uint8_t result_one = test_cpu.get_stack_pointer();
    REQUIRE(result_one == 0x00);

    test_cpu.set_stack_pointer(STACK_START + 1); 
    uint8_t result_two = test_cpu.get_stack_pointer();
    REQUIRE(result_two == 0x01);

    test_cpu.set_stack_pointer(0x0111); 
    uint8_t result_three = test_cpu.get_stack_pointer();
    REQUIRE(result_three == 0x11);
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu - Test program counter getter", "[cpu]") {
    hack_in_test_rom_data(RESET_VECTOR_LOW - PGM_ROM_ADDRESS_SPACE_START, 0x00);
    hack_in_test_rom_data(RESET_VECTOR_HIGH - PGM_ROM_ADDRESS_SPACE_START, 0x80);
    test_cpu.reset(); // reset will go to the reset vector, in this instance will read 0x8000 and skip to that 

    uint16_t result = test_cpu.get_program_counter(); 
    REQUIRE(result == 0x8000); 
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu - Test get accumulator register content getter", "[cpu]") {
    test_cpu.reset();
    REQUIRE(test_cpu.get_accumulator_reg_content() == 0); // temporary fail while we write some code
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu - Test get x index register content getter", "[cpu]") {
    test_cpu.reset();
    REQUIRE(test_cpu.get_x_index_reg_content() == 0); // temporary fail while we write some code
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu - Test get y index register content getter", "[cpu]") {
    test_cpu.reset();
    REQUIRE(test_cpu.get_y_index_reg_content() == 0); // temporary fail while we write some code
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu - Test get stack pointer register content getter", "[cpu]") {
    test_cpu.reset();
    uint8_t stack_start_condition = test_cpu.get_stack_pointer(); 
    REQUIRE(stack_start_condition == 0xFF);
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu - Test get status flag register content getter", "[cpu]") {
    test_cpu.reset(); // reset will go to the reset vector, in this instance will read 0x8000 and skip to that 
    uint8_t status_regs = test_cpu.get_status_flags();
    uint8_t test_start_condition = (1 << ZERO_FLAG) | (1 << IRQ_FLAG) | (1 << BREAK_FLAG) | (1 << UNUSED_FLAG); // the only two flags set should be IRQ disable and unused
    REQUIRE(status_regs == test_start_condition); 
} 

 TEST_CASE_METHOD(emulator_test_fixtures, "cpu - Test read function pointer", "[bus]") {
    // the CPU's read and write functions do nothing, so we test that 
    REQUIRE(test_cpu._read_function_ptr == nullptr);
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu - Test write function pointer", "[bus]") {
    REQUIRE(test_cpu._write_function_ptr == nullptr);
}
