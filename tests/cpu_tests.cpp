#include "catch.hpp"
#include "test_fixtures.h"
    
cpu test_cpu(&test_bus, &test_ram);


TEST_CASE("cpu - Initialize and check defaults", "[cpu]") {
    REQUIRE(0 != 0); // temporary fail while we write some code
}

TEST_CASE("cpu - Test cycle", "[cpu]") {
    // test_cpu.cycle();   
    REQUIRE(0 != 0); // temporary fail while we write some 
}

TEST_CASE("cpu - Test reset", "[cpu]") {
    // put some data into the reset vector, which is a section of ROM memory
    hack_in_test_rom_data(RESET_VECTOR_LOW - ROM_ADDRESS_SPACE_START, 0xDD);
    hack_in_test_rom_data(RESET_VECTOR_HIGH - ROM_ADDRESS_SPACE_START, 0xEE);
    test_cpu.reset();

    // resetting the CPU will set the program counter to the reset vector.
    REQUIRE(test_cpu.get_program_counter() == 0xEEDD);
}

TEST_CASE("cpu - Test IRQ", "[cpu]") {
    // test_cpu.IRQ();     
    REQUIRE(0 != 0); // temporary fail while we write some 
}

TEST_CASE("cpu - Test NMI", "[cpu]") {
    // test_cpu.NMI();    
    REQUIRE(0 != 0); // temporary fail while we write some 
}

TEST_CASE("cpu - Test fetch data", "[cpu]") {
    // test_cpu.fetch_data();  
    REQUIRE(0 != 0); // temporary fail while we write some 
}

TEST_CASE("cpu - Test program counter getter", "[cpu]") {
    //test_cpu.get_program_counter();
    REQUIRE(0 != 0); // temporary fail while we write some code
}

TEST_CASE("cpu - Test get accumulator register content getter", "[cpu]") {
    // test_cpu.get_accumulator_reg_content();
    REQUIRE(0 != 0); // temporary fail while we write some code
}

TEST_CASE("cpu - Test get x index register content getter", "[cpu]") {
    // test_cpu.get_x_index_reg_content();
    REQUIRE(0 != 0); // temporary fail while we write some code
}

TEST_CASE("cpu - Test get y index register content getter", "[cpu]") {
    // test_cpu.get_y_index_reg_content();
    REQUIRE(0 != 0); // temporary fail while we write some code
}

TEST_CASE("cpu - Test get stack pointer register content getter", "[cpu]") {
    // test_cpu.get_stack_pointer_reg_content(); 
    REQUIRE(0 != 0); // temporary fail while we write some code
}

TEST_CASE("cpu - Test get status flag register content getter", "[cpu]") {
    // test_cpu.get_status_reg_contents();
    REQUIRE(0 != 0); // temporary fail while we write some code
} 

 

