#include "catch.hpp"
#include "test_fixtures.h"

uint8_t test_read_fn(void) {
    return 128; // we pass this into our device registration routine, and see what output happens when we read within that space. 
}

void test_write_data_fn(void) {

}

TEST_CASE("bus - Set address", "[bus]") {
    uint16_t test_address = rand() % 0xFFFF;
    test_bus.set_address(test_address);
    REQUIRE(test_bus.read_address() == test_address);
}

TEST_CASE("bus - Set data", "[bus]") {
    uint8_t test_data = rand() % 255;
    test_bus.write_data(test_data);
    REQUIRE(test_bus.read_data() == test_data);
}

TEST_CASE("bus - register new device on bus and test read", "[bus]") {
    test_bus.register_new_bus_device(APU_DISABLED_SPACE_START, APU_DISABLED_SPACE_END, test_read_fn);    
    test_bus.set_address(APU_DISABLED_SPACE_START + 1);
    REQUIRE(test_bus.read_data() == 128); // reading from this address should trigger our test_read_fn, returning 128
}

TEST_CASE("bus - register new device on bus and test write", "[bus]") {
    REQUIRE(0 == 0); 
}
