#pragma once

#include <functional>

#include "memory_map.h"
#include "bus.h"
#include "cpu.h"
#include "ram.h"
#include "cartridge.h"

extern bus test_bus;
extern cpu test_cpu; 
extern ram test_ram;
extern cartridge test_cart;

class emulator_test_fixtures {
public:
    emulator_test_fixtures() {
        test_bus.clear_bus_devices(); // start by clearing the bus because the unit tests don't wipe the memory each time. 
        // register some test devices on the bus
        test_bus.register_new_bus_device(RAM_ADDRESS_SPACE_START, RAM_ADDRESS_SPACE_END, test_ram._read_function_ptr, test_ram._write_function_ptr);    
        test_bus.register_new_bus_device(CART_ADDRESS_SPACE_START, CART_ADDRESS_SPACE_END, test_cart._read_function_ptr);    

        // for testing instructions and hack in the reset vector to be our 0x8000 address and reset the CPU
        test_cart.debug_write_relative(RESET_VECTOR_LOW - PGM_ROM_ADDRESS_SPACE_START, 0x00); 
        test_cart.debug_write_relative(RESET_VECTOR_HIGH - PGM_ROM_ADDRESS_SPACE_START, 0x80);   

        test_cpu.reset();
    }

    ~emulator_test_fixtures() = default;
};

