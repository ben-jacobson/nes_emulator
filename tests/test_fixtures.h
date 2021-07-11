#pragma once

#include "bus.h"
#include "cpu.h"
#include "ram.h"

constexpr uint16_t TEST_RAM_SIZE_BYTES = 2048; // 2Kb for RAM buffer

// test fixtures
extern bus test_bus;
extern ram test_ram;
extern cpu test_cpu; 