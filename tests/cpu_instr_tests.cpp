#include "catch.hpp"
#include "test_fixtures.h"

TEST_CASE_METHOD(emulator_test_fixtures, "cpu instruction - test instruction decoder") {
    // provide the CPU with a bit of data to work with. 
    hack_in_test_rom_data(ROM_ADDRESS_SPACE_START - 0x8000, 0xEA); // NOP instruction
    uint8_t result_rom = test_cart.read(RESET_VECTOR_HIGH);
    CHECK(result_rom == 0x80);

    test_cpu.reset();
    uint16_t program_counter_at_start = test_cpu.get_program_counter();

    // test that the CPU has moved to reset vector
    CHECK(test_cpu.get_program_counter() == 0x8000);

    // call fetch and check that it returned an instruction
    test_cpu.fetch_opcode();
    uint8_t last_fetched_opcode = test_cpu.get_last_fetched_opcode();
    CHECK(last_fetched_opcode == 0xEA);

    // check that fetch has advanced the program counter.
    CHECK(test_cpu.get_program_counter() == program_counter_at_start + 1);

    // check that the name, and cycles are all set correctly. 
    CHECK(test_cpu._opcode_decoder_lookup[0xEA].name == "NOP");
    CHECK(test_cpu._opcode_decoder_lookup[0xEA].cycles_needed == 2);
    REQUIRE(test_cpu._opcode_decoder_lookup[0xEA].instruction != nullptr); // I'd like to have another crack at this later, test that the function has been set to the correct one.
}
    
TEST_CASE_METHOD(emulator_test_fixtures, "cpu instruction - CLI", "[cpu instruction]") {
    test_cpu.instr_CLI(); // clear IRQ disable bit
    REQUIRE(test_cpu.get_status_flags_struct().i == 0);
}


TEST_CASE_METHOD(emulator_test_fixtures, "cpu instruction - PHP", "[cpu instruction]") {
    // test was the status pushed to the stack?
    uint8_t cpu_status_at_start = test_cpu.get_status_flags();
    uint16_t stack_pointer_at_start = STACK_START + test_cpu.get_stack_pointer();  // absolute address
    uint16_t program_counter_at_start = test_cpu.get_program_counter();

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
