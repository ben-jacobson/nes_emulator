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
extern ram another_test_ram; 
extern cartridge test_cart;

class emulator_test_fixtures {
public:
    emulator_test_fixtures() {
        test_bus.clear_bus_devices(); // start by clearing the bus because the unit tests don't wipe the memory each time. 
        // register some test devices on the bus
        test_bus.register_new_bus_device(RAM_ADDRESS_SPACE_START, RAM_ADDRESS_SPACE_END, test_ram._read_function_ptr, test_ram._write_function_ptr);    
        test_bus.register_new_bus_device(RAM_MIRRORA_SPACE_START, RAM_MIRRORA_SPACE_END, another_test_ram._read_function_ptr, another_test_ram._write_function_ptr);    
        test_bus.register_new_bus_device(CART_ADDRESS_SPACE_START, CART_ADDRESS_SPACE_END, test_cart._read_function_ptr);    

        // for testing instructions and hack in the reset vector to be our 0x8000 address and reset the CPU
        hack_in_test_rom_data(RESET_VECTOR_LOW - PGM_ROM_ADDRESS_SPACE_START, 0x00); 
        hack_in_test_rom_data(RESET_VECTOR_HIGH - PGM_ROM_ADDRESS_SPACE_START, 0x80);   

        test_cpu.reset();
    }

    ~emulator_test_fixtures() = default;

    void hack_in_test_rom_data(uint16_t relative_address, uint8_t data) {
        // hacks some data into the rom via it's rom pointer. Uses relative addresses, i.e not in the global address space. Address 0x0000 is is address 0x0000 
        
        uint8_t* cart_ptr = test_cart.get_rom_pointer();   
        *(cart_ptr + relative_address) = data;
    }    
};

