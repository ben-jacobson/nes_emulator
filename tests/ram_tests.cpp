#include "catch.hpp"
#include "test_fixtures.h"

TEST_CASE_METHOD(emulator_test_fixtures, "ram - Check address mapping is valid", "[ram]") {
    REQUIRE(RAM_ADDRESS_SPACE_START + RAM_SIZE_BYTES - 1 == RAM_ADDRESS_SPACE_END);
}

TEST_CASE_METHOD(emulator_test_fixtures, "ram - Create object and set address space", "[ram]") {
    uint16_t address_lower = (rand() % RAM_SIZE_BYTES) - 1;    // theorectially this could underflow, but the test will still pass. 
    uint16_t address_upper = rand() % RAM_SIZE_BYTES;
    ram custom_test_ram(RAM_SIZE_BYTES, address_lower, address_upper); 

    REQUIRE(custom_test_ram._address_space_lower == address_lower);
    REQUIRE(custom_test_ram._address_space_upper == address_upper);
}

TEST_CASE_METHOD(emulator_test_fixtures, "ram - Test custom address mapping", "[ram") {    
    uint8_t test_data = 127;
    uint16_t address_lower = 0x200;
    uint16_t address_upper = 0x400;
    uint16_t test_abs_address = rand() % (address_upper - address_lower + 1) + address_lower;

    CHECK(test_abs_address >= address_lower);
    CHECK(test_abs_address <= address_upper);

    ram custom_test_ram(RAM_SIZE_BYTES, address_lower, address_upper);     // establish another ram device with custom address space
    custom_test_ram.write(test_abs_address, test_data);   // ensure no conflict as we've written to another RAM unit
    uint8_t result = custom_test_ram.read(test_abs_address);
    REQUIRE(result == test_data); // test that offsetting works
}

TEST_CASE_METHOD(emulator_test_fixtures, "ram - Debug read", "[ram]") {
    uint8_t test_data = rand() % 255;
    uint16_t test_address = RAM_ADDRESS_SPACE_START + (rand() % RAM_SIZE_BYTES);

    test_ram.write(test_address, test_data);
    uint8_t result = test_ram.debug_read(test_address - RAM_ADDRESS_SPACE_START);
    REQUIRE(result == test_data);
}

TEST_CASE_METHOD(emulator_test_fixtures, "ram - Read and write test", "[ram]") {
    uint8_t test_data = rand() % 255;
    uint16_t test_address = (rand() % RAM_SIZE_BYTES) + RAM_ADDRESS_SPACE_START;

    // set the address and write the data    
    test_ram.write(test_address, test_data);
    uint8_t result = test_ram.read(test_address);
    REQUIRE(result == test_data);    
}

TEST_CASE_METHOD(emulator_test_fixtures, "ram - Test read function pointer", "[bus]") {
    uint8_t test_data = rand() % 255;
    uint16_t test_address = (rand() % RAM_SIZE_BYTES) + RAM_ADDRESS_SPACE_START;

    // set the address and write the data    
    test_ram.write(test_address, test_data);
    uint8_t result = test_ram._read_function_ptr(test_address);
    REQUIRE(result == test_data);
}

TEST_CASE_METHOD(emulator_test_fixtures, "ram - Test write function pointer", "[bus]") {
    uint8_t test_data = rand() % 255;
    uint16_t test_address = (rand() % RAM_SIZE_BYTES) + RAM_ADDRESS_SPACE_START;

    // set the address and write the data    
    test_ram._write_function_ptr(test_address, test_data);
    uint8_t result = test_ram.read(test_address);
    REQUIRE(result == test_data);
}