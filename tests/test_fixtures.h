#pragma once

#include <functional>

#include "memory_map.h"
#include "bus.h"
#include "cpu.h"
#include "ram.h"
#include "cartridge.h"

// test fixtures
extern bus test_bus;
extern ram test_ram;
extern cpu test_cpu; 
extern cartridge test_cart;

void hack_in_test_rom_data(uint16_t address, uint8_t data);
uint8_t test_read_fn(void);