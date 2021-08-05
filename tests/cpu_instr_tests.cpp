#include "catch.hpp"
#include "test_fixtures.h"


TEST_CASE_METHOD(emulator_test_fixtures, "cpu instruction helper - test check if negative") {
    // signed integers allow for range between -127 and +128 (0x80 wrapped through 0xFF, then up to 0x7F). 
    // this helper function used by instructions sets the flag appropriately with the value provided

    test_cpu.check_if_negative(0x00);       // zero
    CHECK(test_cpu.get_status_flags_struct().n == 0);    

    test_cpu.check_if_negative(0x7F);       // highest positive number 127
    CHECK(test_cpu.get_status_flags_struct().n == 0);

    test_cpu.check_if_negative(0x80);       // -128
    CHECK(test_cpu.get_status_flags_struct().n == 1);

    test_cpu.check_if_negative(0xFE);       // -2
    CHECK(test_cpu.get_status_flags_struct().n == 1);
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu instruction helper - test check if zero") {
    // similar to above, for checking zero 
    test_cpu.check_if_zero(0);       
    CHECK(test_cpu.get_status_flags_struct().z == 1);

    test_cpu.check_if_zero(1);      
    CHECK(test_cpu.get_status_flags_struct().z == 0);

    test_cpu.check_if_zero(255);    
    CHECK(test_cpu.get_status_flags_struct().z == 0);
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu instruction - test instruction decoder") {
    // provide the CPU with a bit of data to work with. 
    test_cart.debug_write_absolute(RESET_VECTOR_HIGH, 0x80); // write a start addressto the high value of the reset vector
    test_cart.debug_write_absolute(0x8000, 0xEA); // write a start addressto the high value of the reset vector
    uint8_t result_rom = test_cart.read(RESET_VECTOR_HIGH);
    CHECK(result_rom == 0x80);

    test_cpu.reset();
    uint16_t program_counter_at_start = test_cpu.get_program_counter();

    // test that the CPU has moved to reset vector
    CHECK(test_cpu.get_program_counter() == 0x8000);

    // call fetch and check that it returned an instruction

    // the cpu takes a number of clock cycles to commence
    for (uint8_t i = 0; i < RESET_CYCLES; i++) {
        test_cpu.cycle();
    }  

    test_cpu.cycle(); // then cycle into the first instruction
    uint8_t last_fetched_opcode = test_cpu.get_last_fetched_opcode();
    CHECK(last_fetched_opcode == 0xEA);

    // check that fetch has advanced the program counter.
    CHECK(test_cpu.get_program_counter() == program_counter_at_start + 1);

    // check that the name, and cycles are all set correctly. 
    CHECK(test_cpu._opcode_decoder_lookup[0xEA].name == "NOP");
    CHECK(test_cpu._opcode_decoder_lookup[0xEA].cycles_needed == 2);
    REQUIRE(test_cpu._opcode_decoder_lookup[0xEA].instruction != nullptr); // I'd like to have another crack at this later, test that the function has been set to the correct one.
}

/*
        Some instruction tests were written, however it was deemed impractical to continue. 
        There exists a number of great test roms for 6502, which have been used to develop the instructions and made these tests a bit redundant
        Writing a unit test for every instruction in all combinations of address modes and processor states just isn't feasible.

        // from here on, we will only test specific use cases, e.g if we think it may cause an issue, or if we're troubleshooting a bug
*/

TEST_CASE_METHOD(emulator_test_fixtures, "cpu instruction - ADC", "[cpu instruction]") {
     // test this instruction using immediate mode

    test_ram.write(0x00FF, 1);        // put 5 into the zero page FF address
    test_cart.load_content_from_stream("69 7F 6D FF 00 69 80", 0x8000); // first instruction is ADC in immediate mode, adding EA. Second instruction is ADC in absolute mode to the value of address FF, the final isntruction is another ADC in immediate which is designed to trigger the overflow flag
    test_cpu.reset(); // reset to a known state
    test_cpu.set_program_counter(0x8000); // reset program counter to 0x8000, bypassing the reset vector
    
    while (!test_cpu.finished_instruction()) { // cycle until the reset 7 cycles has complete
        test_cpu.cycle();
    }

    // check that A register is clear first
    uint8_t result = test_cpu.get_accumulator_reg_content();
    CHECK(result == 0x00);

    // cycle into first ADC instruction, in immediate mode
    test_cpu.cycle(); // first cycle into the ADC instruction, the instruction was to add 7F to the already 0 accumulator value
    result = test_cpu.get_accumulator_reg_content();
    CHECK(result == 0x7F);  // 127 in A
    uint8_t overflow_flag_test = test_cpu.get_status_flags_struct().v;
    CHECK(overflow_flag_test == 0);    
    test_cpu.cycle(); // instruction takes two cycles to complete

    // test this instruction using absolute mode
    test_cpu.cycle(); // cycle into second instruction
    result = test_cpu.get_accumulator_reg_content();
    CHECK(result == 0x80); // 0x7F + 1 = 128, overflowing back to -128 if using unsigned mode
    overflow_flag_test = test_cpu.get_status_flags_struct().v;
    CHECK(overflow_flag_test == 1); 
    test_cpu.cycle();
    test_cpu.cycle();
    test_cpu.cycle(); // absolute mode takes four cycles in total

    uint8_t carry_flag_test = test_cpu.get_status_flags_struct().c;
    CHECK(carry_flag_test == 0);     
    test_cpu.cycle(); // cycle into second instruction
    carry_flag_test = test_cpu.get_status_flags_struct().c;
    CHECK(carry_flag_test == 1);  
    test_cpu.cycle(); // instruction takes two cycles to complete
   
    REQUIRE(test_cpu.finished_instruction() == true); // did we accurately predict the number of cycles
}


/*TEST_CASE_METHOD(emulator_test_fixtures, "cpu instruction - BRK", "[cpu instruction]") {
    uint16_t program_counter_at_start = test_cpu.get_program_counter();
    uint8_t stack_pointer_at_start = test_cpu.get_stack_pointer();

    test_cpu.instr_BRK(); // force break
    uint8_t status_reg_at_start = test_cpu.get_status_flags(); // status reg is pushed to stack with the break flag set, so this needs to be checked after the instruction has run.

    // we should see PC+2 pushed to the stack first 
    test_bus.set_address(STACK_END - stack_pointer_at_start); 
    uint8_t pc_high = test_bus.read_data();
    test_bus.set_address(STACK_END - stack_pointer_at_start - 1);
    uint8_t pc_low = test_bus.read_data();
    uint16_t recreated_pc = (pc_high << 8) | pc_low;
    uint16_t expected_result = program_counter_at_start + 2;
    REQUIRE(recreated_pc == expected_result);

    // the status register should be pushed to the stack
    test_bus.set_address(STACK_END - stack_pointer_at_start - 2);
    uint8_t status_reg_test = test_bus.read_data();
    REQUIRE(status_reg_test == status_reg_at_start);

    // the break flag should be set to 1
    status_reg_test = test_cpu.get_status_flags_struct().b;
    REQUIRE(status_reg_test == 1);
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu instruction - CLI", "[cpu instruction]") {
    test_cpu.instr_CLI(); // clear IRQ disable bit
    REQUIRE(test_cpu.get_status_flags_struct().i == 0);
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu instruction - DEX", "[cpu instruction]") {
    // scenario 1 - value larger than 1 being decremented
    test_cpu.debug_set_x_register(5);
    uint8_t expected_val = test_cpu.get_x_index_reg_content() - 1; // can underflow if needs be
    test_cpu.instr_DEX();
    uint8_t result = test_cpu.get_x_index_reg_content();
    CHECK(test_cpu.get_status_flags_struct().z == 0);
    CHECK(test_cpu.get_status_flags_struct().n == 0);
    CHECK(result == expected_val);

    // scenario 2 - value of one, decrementing to zero
    test_cpu.debug_set_x_register(1);
    expected_val = test_cpu.get_x_index_reg_content() - 1; // can underflow if needs be
    test_cpu.instr_DEX();
    result = test_cpu.get_x_index_reg_content();
    CHECK(test_cpu.get_status_flags_struct().z == 1);
    CHECK(test_cpu.get_status_flags_struct().n == 0);
    CHECK(result == expected_val);

    // scenarion 3 - zero going negative
    test_cpu.debug_set_x_register(0);
    expected_val = test_cpu.get_x_index_reg_content() - 1; // can underflow if needs be
    test_cpu.instr_DEX();
    result = test_cpu.get_x_index_reg_content();
    CHECK(test_cpu.get_status_flags_struct().z == 0);
    CHECK(test_cpu.get_status_flags_struct().n == 1);
    CHECK(result == expected_val);
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu instruction - DEY", "[cpu instruction]") {
    // scenario 1 - value larger than 1 being decremented
    test_cpu.debug_set_y_register(5);
    uint8_t expected_val = test_cpu.get_y_index_reg_content() - 1; // can underflow if needs be
    test_cpu.instr_DEY();
    uint8_t result = test_cpu.get_y_index_reg_content();
    CHECK(test_cpu.get_status_flags_struct().z == 0);
    CHECK(test_cpu.get_status_flags_struct().n == 0);
    CHECK(result == expected_val);

    // scenario 2 - value of one, decrementing to zero
    test_cpu.debug_set_y_register(1);
    expected_val = test_cpu.get_y_index_reg_content() - 1; // can underflow if needs be
    test_cpu.instr_DEY();
    result = test_cpu.get_y_index_reg_content();
    CHECK(test_cpu.get_status_flags_struct().z == 1);
    CHECK(test_cpu.get_status_flags_struct().n == 0);
    CHECK(result == expected_val);

    // scenarion 3 - zero going negative
    test_cpu.debug_set_y_register(0);
    expected_val = test_cpu.get_y_index_reg_content() - 1; // can underflow if needs be
    test_cpu.instr_DEY();
    result = test_cpu.get_y_index_reg_content();
    CHECK(test_cpu.get_status_flags_struct().z == 0);
    CHECK(test_cpu.get_status_flags_struct().n == 1);
    CHECK(result == expected_val);
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu instruction - INX", "[cpu instruction]") {
    // scenario 1 - value of FF 1 being incremented
    test_cpu.debug_set_x_register(0xFF);
    test_cpu.instr_INX();
    uint8_t result = test_cpu.get_x_index_reg_content();
    CHECK(test_cpu.get_status_flags_struct().z == 1);
    CHECK(test_cpu.get_status_flags_struct().n == 0);
    CHECK(result == 0);

    // scenario 2 - value within range, incrementing, but crossing into negative number (twos complement)
    test_cpu.debug_set_x_register(0x7F);
    test_cpu.instr_INX();
    result = test_cpu.get_x_index_reg_content();
    CHECK(test_cpu.get_status_flags_struct().z == 0);
    CHECK(test_cpu.get_status_flags_struct().n == 1);
    CHECK(result == 0x80);
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu instruction - INY", "[cpu instruction]") {
    // scenario 1 - value of FF 1 being incremented
    test_cpu.debug_set_y_register(0xFF);
    test_cpu.instr_INY();
    uint8_t result = test_cpu.get_y_index_reg_content();
    CHECK(test_cpu.get_status_flags_struct().z == 1);
    CHECK(test_cpu.get_status_flags_struct().n == 0);
    CHECK(result == 0);

    // scenario 2 - value within range, incrementing, but crossing into negative number (twos complement)
    test_cpu.debug_set_y_register(0x7F);
    test_cpu.instr_INY();
    result = test_cpu.get_y_index_reg_content();
    CHECK(test_cpu.get_status_flags_struct().z == 0);
    CHECK(test_cpu.get_status_flags_struct().n == 1);
    CHECK(result == 0x80);
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu instruction - NOP", "[cpu instruction]") {
    // NOP does nothing!
    REQUIRE(test_cpu.instr_NOP() == 0);
}
TEST_CASE_METHOD(emulator_test_fixtures, "cpu instruction - PHP", "[cpu instruction]") {
    // test was the status pushed to the stack?
    uint8_t cpu_status_at_start = test_cpu.get_status_flags();
    uint16_t stack_pointer_at_start = STACK_START + test_cpu.get_stack_pointer();  // absolute address

    // run the instruction
    test_cpu.instr_PHP();

    // check that the stack pointer was decremented upon push
    uint16_t stack_pointer_now = STACK_START + test_cpu.get_stack_pointer();
    CHECK(stack_pointer_now == stack_pointer_at_start - 1);

    // check to see if stack was updated with program status
    uint8_t result_stack_contents = test_ram.read(stack_pointer_at_start);
    REQUIRE(result_stack_contents == cpu_status_at_start);
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu instruction - SEI", "[cpu instruction]") {
    test_cpu.instr_SEI(); // set IRQ disable bit
    REQUIRE(test_cpu.get_status_flags_struct().i == 1);
}
*/

