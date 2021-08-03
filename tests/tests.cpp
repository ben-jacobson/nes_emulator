#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "test_fixtures.h"

bus test_bus;
ram test_ram(RAM_SIZE_BYTES, RAM_ADDRESS_SPACE_START, RAM_ADDRESS_SPACE_END);
ram another_test_ram(RAM_SIZE_BYTES, 0x4000, 0x4000 + RAM_SIZE_BYTES); 
cartridge test_cart(CART_ADDRESS_SPACE_START, CART_ADDRESS_SPACE_END);
cpu test_cpu(&test_bus);
