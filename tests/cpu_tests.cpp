#include "catch.hpp"
#include "test_fixtures.h"
    
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

TEST_CASE_METHOD(emulator_test_fixtures, "cpu - Initialize and check defaults", "[cpu]") {
    REQUIRE(0 != 0); // temporary fail while we write some code
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu - Test cycle", "[cpu]") {
    // test_cpu->cycle();   
    REQUIRE(0 != 0); // temporary fail while we write some 
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu - Test reset", "[cpu]") {
    // put some data into the reset vector, which is a section of ROM memory
    hack_in_test_rom_data(RESET_VECTOR_LOW - ROM_ADDRESS_SPACE_START, 0xDD);
    hack_in_test_rom_data(RESET_VECTOR_HIGH - ROM_ADDRESS_SPACE_START, 0xEE);

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
    test_cpu.instr_CLI(); // cpu defaults to IRQ disabled, clear this bit to enable IRQ
    hack_in_test_rom_data(0xFFFF - ROM_ADDRESS_SPACE_START, 0xBB); 
    hack_in_test_rom_data(0xFFFE - ROM_ADDRESS_SPACE_START, 0xAA);
    CHECK(test_cpu.get_status_flags_struct().i == 0); // is the interrupt enabled (0)?
    
    uint8_t stack_pointer_at_start = test_cpu.get_stack_pointer();
    uint16_t program_counter_at_start = test_cpu.get_program_counter();    
    bool result = test_cpu.IRQ();   
    CHECK(result == true);

    // program counter should now be on the stack
    uint8_t result_pgm_counter_high = test_ram.read(stack_pointer_at_start); // these tests are incomplete, and left to fail 
    CHECK(result_pgm_counter_high == (program_counter_at_start & 0xFF00));
    uint8_t result_pgm_counter_low = test_ram.read(stack_pointer_at_start - 1);
    CHECK(result_pgm_counter_low == (program_counter_at_start & 0x00FF));
        
    // also status registers
    uint8_t result_stack_pointer_status_regs = test_ram.read(STACK_START + stack_pointer_at_start - 2);
    CHECK(result_stack_pointer_status_regs == test_cpu.get_status_flags());

    REQUIRE(test_cpu.get_program_counter() == 0xBBAA); // was the program counter set to the IRQ vector?
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu - Test NMI", "[cpu]") {
    hack_in_test_rom_data(0xFFFB - ROM_ADDRESS_SPACE_START, 0xEE);
    hack_in_test_rom_data(0xFFFA - ROM_ADDRESS_SPACE_START, 0xFF); 

    uint8_t stack_pointer_at_start = test_cpu.get_stack_pointer();
    uint16_t program_counter_at_start = test_cpu.get_program_counter();        
    bool result = test_cpu.NMI();   
    CHECK(result == true);

    // program counter should now be on the stack
    uint8_t result_pgm_counter_high = test_ram.read(stack_pointer_at_start); // these tests are incomplete, and left to fail 
    CHECK(result_pgm_counter_high == (program_counter_at_start & 0xFF00));
    uint8_t result_pgm_counter_low = test_ram.read(stack_pointer_at_start - 1);
    CHECK(result_pgm_counter_low == (program_counter_at_start & 0x00FF));
        
    // also status registers
    uint8_t result_stack_pointer_status_regs = test_ram.read(STACK_START + stack_pointer_at_start - 2);
    CHECK(result_stack_pointer_status_regs == test_cpu.get_status_flags());

    REQUIRE(test_cpu.get_program_counter() == 0xEEFF);  // was the program counter set to the NMI vector?
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

TEST_CASE_METHOD(emulator_test_fixtures, "cpu - Test fetch data", "[cpu]") {
    // test_cpu->fetch_data();  
    REQUIRE(0 != 0); // temporary fail while we write some 
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu - Test program counter getter", "[cpu]") {
    //test_cpu->get_program_counter();
    REQUIRE(0 != 0); // temporary fail while we write some code
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu - Test get accumulator register content getter", "[cpu]") {
    // test_cpu->get_accumulator_reg_content();
    REQUIRE(0 != 0); // temporary fail while we write some code
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu - Test get x index register content getter", "[cpu]") {
    // test_cpu->get_x_index_reg_content();
    REQUIRE(0 != 0); // temporary fail while we write some code
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu - Test get y index register content getter", "[cpu]") {
    // test_cpu->get_y_index_reg_content();
    REQUIRE(0 != 0); // temporary fail while we write some code
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu - Test get stack pointer register content getter", "[cpu]") {
    // test_cpu->get_stack_pointer_reg_content(); 
    REQUIRE(0 != 0); // temporary fail while we write some code
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu - Test get status flag register content getter", "[cpu]") {
    // test_cpu->get_status_reg_contents();
    REQUIRE(0 != 0); // temporary fail while we write some code
} 

 TEST_CASE_METHOD(emulator_test_fixtures, "cpu - Test read function pointer", "[bus]") {
    // the CPU's read and write functions do nothing, so we test that 
    REQUIRE(test_cpu._read_function_ptr == nullptr);
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu - Test write function pointer", "[bus]") {
    REQUIRE(test_cpu._write_function_ptr == nullptr);
}
