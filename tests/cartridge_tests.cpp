#include "catch.hpp"
#include "test_fixtures.h"

TEST_CASE_METHOD(emulator_test_fixtures, "rom - Check address mapping is valid for cartridge and rom", "[cartridge]") {
    REQUIRE(CART_ADDRESS_SPACE_START + CART_SIZE_BYTES - 1 == CART_ADDRESS_SPACE_END);
    REQUIRE(PGM_ROM_ADDRESS_SPACE_START + PGM_ROM_SIZE_BYTES - 1 == PGM_ROM_ADDRESS_SPACE_END);    
}

TEST_CASE_METHOD(emulator_test_fixtures, "rom - Test hack in rom data helper function", "[cartridge]") {
    uint16_t test_relative_address;  
    uint8_t test_data;

    for (uint8_t i = 0; i < 8; i++) {  // test this in 8 random places across the ROM
        test_relative_address = rand() % PGM_ROM_SIZE_BYTES;
        test_data = rand() % 255;
        
        test_cart.debug_write(test_relative_address, test_data);
        REQUIRE(test_cart.debug_read(test_relative_address) == test_data); // a little redundant, we have a separate test below for this. can't hurt. 
    }
}

TEST_CASE_METHOD(emulator_test_fixtures, "rom - Create object and set address space", "[cartridge]") {
    uint16_t address_lower = (rand() % PGM_ROM_SIZE_BYTES) - 1;    // theorectially this could underflow, but the test will still pass. 
    uint16_t address_upper = rand() % PGM_ROM_SIZE_BYTES;
    cartridge another_test_cart(address_lower, address_upper); 

    CHECK(another_test_cart._address_space_lower == address_lower);
    REQUIRE(another_test_cart._address_space_upper == address_upper);
}

TEST_CASE_METHOD(emulator_test_fixtures, "rom - Debug read", "[cartridge]") {
    uint8_t test_data = rand() % 255;
    uint16_t test_address = rand() % PGM_ROM_SIZE_BYTES;

    test_cart.debug_write(test_address, test_data);    
    REQUIRE(test_cart.debug_read(test_address) == test_data);
}

TEST_CASE_METHOD(emulator_test_fixtures, "rom - Read test", "[ram]") {
    uint8_t test_data = rand() % 255;
    uint16_t test_address = (rand() % RAM_SIZE_BYTES) + PGM_ROM_ADDRESS_SPACE_START;

    // set the address and write the data    
    test_cart.debug_write(test_address - PGM_ROM_ADDRESS_SPACE_START, test_data);    
    uint8_t result = test_cart.read_rom(test_address);
    REQUIRE(result == test_data);    
}

TEST_CASE_METHOD(emulator_test_fixtures, "rom - Test read function pointer", "[bus]") {
    uint8_t test_data = rand() % 255;
    uint16_t test_address = PGM_ROM_ADDRESS_SPACE_START + (rand() % PGM_ROM_SIZE_BYTES);

    // set the address and write the data    
    test_cart.debug_write(test_address - PGM_ROM_ADDRESS_SPACE_START, test_data);    
    uint8_t result = test_cart._read_function_ptr(test_address);
    REQUIRE(result == test_data);
}

TEST_CASE_METHOD(emulator_test_fixtures, "rom - Test write function pointer", "[bus]") {
    REQUIRE(test_cart._write_function_ptr == nullptr);
}