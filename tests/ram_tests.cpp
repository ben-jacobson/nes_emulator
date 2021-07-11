#include "catch.hpp"
#include "test_fixtures.h"

// test fixtures
bus test_bus;
ram test_ram(&test_bus, TEST_RAM_SIZE_BYTES, 0x000, 0x7FF);

TEST_CASE("ram - Create object and set address space", "[ram]") {
    uint16_t address_lower = (rand() % TEST_RAM_SIZE_BYTES) - 1;    // theorectially this could underflow, but the test will still pass. 
    uint16_t address_upper = rand() % TEST_RAM_SIZE_BYTES;
    ram another_test_ram(&test_bus, TEST_RAM_SIZE_BYTES, address_lower, address_upper); 

    REQUIRE(another_test_ram._address_space_lower == address_lower);
    REQUIRE(another_test_ram._address_space_upper == address_upper);
}
TEST_CASE("ram - Test address mapping", "[ram") {    
    uint8_t test_data = 127;
    uint16_t address_lower = 0x200;
    uint16_t address_upper = 0x400;
    uint16_t test_abs_address = rand() % (address_upper - address_lower + 1) + address_lower;
    REQUIRE(test_abs_address >= address_lower);
    REQUIRE(test_abs_address <= address_upper);

    ram another_test_ram(&test_bus, TEST_RAM_SIZE_BYTES, address_lower, address_upper);     

    test_bus.set_address(test_abs_address);
    test_bus.write_data(test_data);
    another_test_ram.write();

    test_bus.set_address(test_abs_address); // set the address once again and read it back
    test_bus.write_data(10); // write something else on the bus to ensure it is being overwritten by the ram device
    another_test_ram.read();    // place the ram data back on the bus overwriting what we wrote

    REQUIRE(test_bus.read_data() == test_data);                 
    REQUIRE(another_test_ram.debug_read(test_abs_address - address_lower) == test_data); // test that offsetting works
}

TEST_CASE("ram - Debug read", "[ram]") {
    uint8_t test_data = rand() % 255;
    uint16_t test_address = rand() % TEST_RAM_SIZE_BYTES;

    test_bus.set_address(test_address);
    test_bus.write_data(test_data);
    test_ram.write();
    REQUIRE(test_ram.debug_read(test_address) == test_data);
}

TEST_CASE("ram - Read and write test", "[ram]") {
    uint8_t test_data = rand() % 255;
    uint16_t test_address = rand() % TEST_RAM_SIZE_BYTES;

    // set the address and write the data
    test_bus.set_address(test_address);
    test_bus.write_data(test_data);
    test_ram.write();

    // set the address once again and read it back
    test_bus.set_address(test_address);
    test_bus.write_data(rand() % 255); // write something else on the bus to ensure it is being overwritten by the ram device
    test_ram.read();    // place it on the bus

    REQUIRE(test_bus.read_data() == test_data);    
}