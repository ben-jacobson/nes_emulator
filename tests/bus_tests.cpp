#include "catch.hpp"
#include "test_fixtures.h"


TEST_CASE_METHOD(emulator_test_fixtures, "bus - Test for correct registration order") {
    // our test fixture has registered three devices for us, like this. RAM_ADDRESS_SPACE, RAM_MIRRORA_SPACE and CART_ADDRESS_SPACE. We'll register one more and check the order.
    test_bus.register_new_bus_device(0x0000, 0xFFFF, nullptr);
    REQUIRE(test_bus.devices_connected_to_bus[0]._address_range_start == RAM_ADDRESS_SPACE_START);
    REQUIRE(test_bus.devices_connected_to_bus[1]._address_range_start == CART_ADDRESS_SPACE_START);
    REQUIRE(test_bus.devices_connected_to_bus[2]._address_range_start == 0x0000);
    REQUIRE(test_bus.device_index == 3);
}

TEST_CASE_METHOD(emulator_test_fixtures, "bus - test that reading any given address doesn't cause crash") {
    // we encountered an issue where reading from address a few various address would cause a crash. 
    for (uint16_t i = 0x00; i < 0xFFFF; i++) {
        if (i > 41000) {
            std::cout << (uint16_t)i << ", ";
        }
        test_bus.set_address(i);
        test_bus.read_data();
	}
    REQUIRE(1 == 1); // if you got this far without a crash, you're golden!
}

TEST_CASE_METHOD(emulator_test_fixtures, "bus - Test for invalid memory location") {
    test_bus.set_address(0x1234); // on my linux machine this causes a segmentation fault.
    uint8_t result = test_bus.read_data();
    REQUIRE(result == 0);
}

TEST_CASE_METHOD(emulator_test_fixtures, "bus - Test writing to read only device") {
    test_bus.set_address(PGM_ROM_ADDRESS_SPACE_START + 10); // this will revert to our cartridge, which is read only
    test_bus.write_data(0xFF);
    uint8_t result = test_bus.read_data();
    REQUIRE(result == 0x00); // as in the write function failed, did not crash the program or throw an exception
}

TEST_CASE_METHOD(emulator_test_fixtures, "bus - Test bus device clearing function") {
    // the test fixture registered three devices
    test_bus.clear_bus_devices();
    REQUIRE(test_bus.device_index == 0);
    REQUIRE(test_bus.devices_connected_to_bus[0]._address_range_start == 0);
}

TEST_CASE_METHOD(emulator_test_fixtures, "bus - Test get_index_of_connected_device ordering") {
    // our test fixture has registered three devices for us, like this. RAM_ADDRESS_SPACE, RAM_MIRRORA_SPACE and CART_ADDRESS_SPACE. let's test our index getter
    REQUIRE(test_bus.devices_connected_to_bus[test_bus.get_index_of_connected_device(RAM_ADDRESS_SPACE_START + 1)]._address_range_start == RAM_ADDRESS_SPACE_START);
    REQUIRE(test_bus.devices_connected_to_bus[test_bus.get_index_of_connected_device(CART_ADDRESS_SPACE_START + 1)]._address_range_start == CART_ADDRESS_SPACE_START);    

    REQUIRE(test_bus.get_index_of_connected_device(RAM_ADDRESS_SPACE_START + 1) == 0);
    REQUIRE(test_bus.get_index_of_connected_device(CART_ADDRESS_SPACE_START + 1) == 1);    
    REQUIRE(test_bus.device_index == 2);
}

TEST_CASE_METHOD(emulator_test_fixtures, "bus - Test bus decoding set address and nothing else", "[bus]") {
    uint16_t test_address = rand() % 0xFFFF;
    test_bus.set_address(test_address);
    REQUIRE(test_bus.read_address() == test_address);
}

TEST_CASE_METHOD(emulator_test_fixtures, "bus - Test bus decoding read and write using preconfigured device", "[bus]") {
    test_bus.set_address(RAM_ADDRESS_SPACE_START + 1);
    test_bus.write_data(99);
    uint8_t test_read_data = test_bus.read_data(); 
    REQUIRE(test_read_data == 99); // reading from this address should trigger our test_read_fn, returning 128
}