#include "catch.hpp"
#include "test_fixtures.h"


TEST_CASE("bus - test bus decoding behaviour", "[bus]") {

    // register our test ram device
    test_bus.register_new_bus_device(RAM_ADDRESS_SPACE_START, RAM_ADDRESS_SPACE_END, std::bind(&ram::read, &test_ram, 0), std::bind(&ram::write, &test_ram, std::placeholders::_1, std::placeholders::_2));    
    
    // create an additional device for testing and register it
    ram another_test_ram(&test_bus, RAM_SIZE_BYTES, RAM_MIRRORA_SPACE_START, RAM_MIRRORA_SPACE_END); 
    test_bus.register_new_bus_device(RAM_MIRRORA_SPACE_START, RAM_MIRRORA_SPACE_END, std::bind(&ram::read, &another_test_ram, 0), std::bind(&ram::write, &another_test_ram, std::placeholders::_1, std::placeholders::_2));    

    SECTION("Set address", "[bus]") {
        uint16_t test_address = rand() % 0xFFFF;
        test_bus.set_address(test_address);
        REQUIRE(test_bus.read_address() == test_address);
    }

    SECTION("Test read and write using preconfigured device", "[bus]") {
        test_bus.set_address(RAM_ADDRESS_SPACE_START + 1);
        test_bus.write_data(99);
        uint8_t test_read_data = test_bus.read_data(); 
        REQUIRE(test_read_data == 99); // reading from this address should trigger our test_read_fn, returning 128
    }

    SECTION("Test read and write using an additional device", "[bus]") {
        test_bus.set_address(RAM_MIRRORA_SPACE_START + 1);
        test_bus.write_data(105);
        uint8_t test_read_data = test_bus.read_data(); 
        REQUIRE(test_read_data == 105); // reading from this address should trigger our test_read_fn, returning 128
    }
}