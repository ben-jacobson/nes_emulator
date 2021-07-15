#include "catch.hpp"
#include "test_fixtures.h"

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
