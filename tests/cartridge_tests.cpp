#include "catch.hpp"
#include "test_fixtures.h"

// test fixtures
cartridge test_cart(&test_bus, CART_ADDRESS_SPACE_START, CART_ADDRESS_SPACE_END);

void hack_in_test_rom_data(uint16_t relative_address, uint8_t data) {
    // hacks some data into the rom via it's rom pointer. Uses relative addresses, i.e not in the global address space. Address 0x0000 is is address 0x0000 
    uint8_t* cart_ptr = test_cart.get_rom_pointer();    
    //cart_ptr[relative_address] = data; 
    *(cart_ptr + relative_address) = data;
}

TEST_CASE("cartridge - Check address mapping is valid for cartridge and rom", "[cartridge]") {
    REQUIRE(CART_ADDRESS_SPACE_START + CART_SIZE_BYTES - 1 == CART_ADDRESS_SPACE_END);
    REQUIRE(ROM_ADDRESS_SPACE_START + ROM_SIZE_BYTES - 1 == ROM_ADDRESS_SPACE_END);    
}

TEST_CASE("cartridge - Test hack in rom data helper function", "[cartridge]") {
    uint16_t test_relative_address;  
    uint8_t test_data;

    for (uint8_t i = 0; i < 8; i++) {  // test this in 8 random places across the ROM
        test_relative_address = rand() % ROM_SIZE_BYTES;
        test_data = rand() % 255;

        hack_in_test_rom_data(test_relative_address, test_data);
        test_bus.set_address(ROM_ADDRESS_SPACE_START + test_relative_address);
        test_cart.read();
        REQUIRE(test_bus.read_data() == test_data);
    }
}

TEST_CASE("cartridge - Create object and set address space", "[cartridge]") {
    uint16_t address_lower = (rand() % ROM_SIZE_BYTES) - 1;    // theorectially this could underflow, but the test will still pass. 
    uint16_t address_upper = rand() % ROM_SIZE_BYTES;
    cartridge another_test_cart(&test_bus, address_lower, address_upper); 

    REQUIRE(another_test_cart._address_space_lower == address_lower);
    REQUIRE(another_test_cart._address_space_upper == address_upper);
}

TEST_CASE("cartridge - Test custom address mapping", "[cartridge") {    
    uint8_t test_data = 127;
    uint16_t address_lower = 0x200;
    uint16_t address_upper = 0x400;
    uint16_t test_abs_address = rand() % (address_upper - address_lower + 1) + address_lower;
    REQUIRE(test_abs_address >= address_lower);
    REQUIRE(test_abs_address <= address_upper);

    cartridge another_test_cart(&test_bus, address_lower, address_upper); 
    hack_in_test_rom_data(test_abs_address - ROM_ADDRESS_SPACE_START, test_data);    

    test_bus.set_address(test_abs_address); // set the address once again and read it back
    test_bus.write_data(10); // write something else on the bus to ensure it is being overwritten by the ram device
    another_test_cart.read();    // place the ram data back on the bus overwriting what we wrote

    REQUIRE(test_bus.read_data() == test_data);                 
    REQUIRE(another_test_cart.debug_read(test_abs_address - address_lower) == test_data); // test that offsetting works
}

TEST_CASE("cartridge - Debug read", "[cartridge]") {
    uint8_t test_data = rand() % 255;
    uint16_t test_address = rand() % ROM_SIZE_BYTES;

    hack_in_test_rom_data(test_address, test_data);    
    REQUIRE(test_cart.debug_read(test_address) == test_data);
}

TEST_CASE("cartridge - Read test places data on bus", "[cartridge]") {
    uint8_t test_data = rand() % 255;
    uint16_t test_abs_address = ROM_ADDRESS_SPACE_START + (rand() % ROM_SIZE_BYTES);

    // set the address and write the data
    hack_in_test_rom_data(test_abs_address, test_data);    
    
    // set the address once again and read it back
    test_bus.set_address(test_abs_address);
    test_bus.write_data(rand() % 255); // write something else on the bus to ensure it is being overwritten by the ram device
    test_cart.read();    // place the cart data on the bus

    REQUIRE(test_bus.read_data() == test_data);    
}