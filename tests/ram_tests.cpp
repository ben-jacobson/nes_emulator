#include "catch.hpp"
#include "test_fixtures.h"

TEST_CASE_METHOD(emulator_test_fixtures, "ram - Check address mapping is valid", "[ram]") {
    REQUIRE(RAM_ADDRESS_SPACE_START + RAM_SIZE_BYTES == RAM_SIZE_BYTES);
}

TEST_CASE_METHOD(emulator_test_fixtures, "ram - Test ram mirroring", "[ram]") {
    // Ram runs between 0x0000 and 0x07FF, with three mirrors, first 0x0800-0x0FFF, second 0x1000-0x17FF, and third 0x1800-0x1FFF

    test_ram.write(0x0000, 0xAB);
    CHECK(test_ram.read(0x0000) == 0xAB);
    CHECK(test_ram.read(0x0800) == 0xAB);
    CHECK(test_ram.read(0x1000) == 0xAB);
    CHECK(test_ram.read(0x1800) == 0xAB);

    test_ram.write(0x0100, 0xCC);
    CHECK(test_ram.read(0x0100) == 0xCC);
    CHECK(test_ram.read(0x0900) == 0xCC);
    CHECK(test_ram.read(0x1100) == 0xCC);
    CHECK(test_ram.read(0x1900) == 0xCC);

    test_ram.write(0x07FF, 0xEE);
    CHECK(test_ram.read(0x07FF) == 0xEE);
    CHECK(test_ram.read(0x0FFF) == 0xEE);
    CHECK(test_ram.read(0x17FF) == 0xEE);
    CHECK(test_ram.read(0x1FFF) == 0xEE);    

    test_ram.write(0x1FFF, 0xAA);   // writing something at the end should also work to write to previously mapped addresses too.
    CHECK(test_ram.read(0x07FF) == 0xAA);
    CHECK(test_ram.read(0x0FFF) == 0xAA);
    CHECK(test_ram.read(0x17FF) == 0xAA);
    CHECK(test_ram.read(0x1FFF) == 0xAA);        
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