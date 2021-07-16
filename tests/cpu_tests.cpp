#include "catch.hpp"
#include "test_fixtures.h"
    
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
    uint16_t result = test_cpu.get_program_counter();   
    // resetting the CPU will set the program counter to the reset vector.
    REQUIRE(result == 0xEEDD); // check that this copied into program counter
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu - Test IRQ", "[cpu]") {
    // test_cpu->IRQ();     
    REQUIRE(0 != 0); // temporary fail while we write some 
}

TEST_CASE_METHOD(emulator_test_fixtures, "cpu - Test NMI", "[cpu]") {
    // test_cpu->NMI();    
    REQUIRE(0 != 0); // temporary fail while we write some 
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
