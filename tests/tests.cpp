#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "test_fixtures.h"

// CPU fixtures
bus test_bus;
ram test_ram(RAM_SIZE_BYTES, RAM_ADDRESS_SPACE_START, RAM_ADDRESS_SPACE_END);
cartridge test_cart(CART_ADDRESS_SPACE_START, CART_ADDRESS_SPACE_END);
apu_io test_apu_io(APUIO_ADDRESS_SPACE_START, APUIO_ADDRESS_SPACE_END);
cpu test_cpu(&test_bus);

// PPU fixtures
bus test_ppu_bus;
ppu test_ppu(&test_bus, &test_ppu_bus, &test_cpu);
ram test_palette_ram(PALETTE_RAM_SIZE, PALETTE_RAM_INDEX_START, PALETTE_RAM_MIRROR_END);

// Nametables or VRAM fixtures
ram test_nametable_0_memory(NAMETABLE_SIZE, NAMETABLE_0_START, NAMETABLE_0_END);
ram test_nametable_1_memory(NAMETABLE_SIZE, NAMETABLE_1_START, NAMETABLE_1_END);
ram test_nametable_2_memory(NAMETABLE_SIZE, NAMETABLE_2_START, NAMETABLE_2_END);
ram test_nametable_3_memory(NAMETABLE_SIZE, NAMETABLE_3_START, NAMETABLE_3_END);