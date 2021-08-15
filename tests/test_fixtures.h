#pragma once

#include <functional>

#include "memory_map.h"
#include "bus.h"
#include "cpu.h"
#include "ppu.h"
#include "ram.h"
#include "cartridge.h"

extern bus test_bus, test_ppu_bus;
extern cpu test_cpu; 
extern ram test_ram;
extern cartridge test_cart;
extern ppu test_ppu;
extern ram test_palette_ram;
extern ram test_nametable_0_memory, test_nametable_1_memory, test_nametable_2_memory, test_nametable_3_memory;

class emulator_test_fixtures {
public:
    emulator_test_fixtures() {
        test_bus.clear_bus_devices(); // start by clearing the bus because the unit tests don't wipe the memory each time.
        test_ppu_bus.clear_bus_devices();

        // register some test devices on the bus
        test_bus.register_new_bus_device(RAM_ADDRESS_SPACE_START, RAM_ADDRESS_SPACE_END, test_ram._read_function_ptr, test_ram._write_function_ptr);    
        test_bus.register_new_bus_device(CART_ADDRESS_SPACE_START, CART_ADDRESS_SPACE_END, test_cart._read_function_ptr);    

         // register the PPU to the CPU bus
        test_bus.register_new_bus_device(PPU_ADDRESS_SPACE_START, PPU_MIRROR_SPACE_END, test_ppu._read_function_ptr, test_ppu._write_function_ptr);

        // register our ppu bus devices
        test_ppu_bus.register_new_bus_device(CHR_ROM_START, CHR_ROM_END, test_cart._ppu_read_function_ptr, test_cart._ppu_write_function_ptr);
        test_ppu_bus.register_new_bus_device(PALETTE_RAM_INDEX_START, PALETTE_RAM_MIRROR_END, test_palette_ram._read_function_ptr, test_palette_ram._write_function_ptr);

        test_ppu_bus.register_new_bus_device(NAMETABLE_0_START, NAMETABLE_0_END, test_nametable_0_memory._read_function_ptr, test_nametable_0_memory._write_function_ptr);
        test_ppu_bus.register_new_bus_device(NAMETABLE_1_START, NAMETABLE_1_END, test_nametable_1_memory._read_function_ptr, test_nametable_1_memory._write_function_ptr);
        test_ppu_bus.register_new_bus_device(NAMETABLE_2_START, NAMETABLE_2_END, test_nametable_2_memory._read_function_ptr, test_nametable_2_memory._write_function_ptr);
        test_ppu_bus.register_new_bus_device(NAMETABLE_3_START, NAMETABLE_3_END, test_nametable_3_memory._read_function_ptr, test_nametable_3_memory._write_function_ptr);

        test_ppu.reset();

        // for testing instructions and hack in the reset vector to be our 0x8000 address and reset the CPU
        test_cart.debug_write_relative(RESET_VECTOR_LOW - PGM_ROM_ADDRESS_SPACE_START, 0x00); 
        test_cart.debug_write_relative(RESET_VECTOR_HIGH - PGM_ROM_ADDRESS_SPACE_START, 0x80);   
        test_cpu.reset();
    }

    ~emulator_test_fixtures() = default;
};

