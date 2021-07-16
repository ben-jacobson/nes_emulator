#include "catch.hpp"
#include "test_fixtures.h"

TEST_CASE_METHOD(emulator_test_fixtures, "bus - test bus decoding, set address", "[bus]") {
    uint16_t test_address = rand() % 0xFFFF;
    test_bus.set_address(test_address);
    REQUIRE(test_bus.read_address() == test_address);
}

TEST_CASE_METHOD(emulator_test_fixtures, "bus - test bus decoding read and write using preconfigured device", "[bus]") {
    test_bus.set_address(RAM_ADDRESS_SPACE_START + 1);
    test_bus.write_data(99);
    uint8_t test_read_data = test_bus.read_data(); 
    REQUIRE(test_read_data == 99); // reading from this address should trigger our test_read_fn, returning 128
}

TEST_CASE_METHOD(emulator_test_fixtures, "bus - test bus decoding read and write using an additional device", "[bus]") {
    test_bus.set_address(RAM_MIRRORA_SPACE_START + 1);
    test_bus.write_data(105);
    uint8_t test_read_data = test_bus.read_data(); 
    REQUIRE(test_read_data == 105); // reading from this address should trigger our test_read_fn, returning 128
}
