#include "catch.hpp"
#include "test_fixtures.h"
    
TEST_CASE_METHOD(emulator_test_fixtures, "ppu - Test read and write to address space", "[ppu]") {
    // writing to the OAM data register simply sets the value with what we provided in memory. It is a good test candidate as has read and write access
    test_bus.set_address(OAMDATA); 
    test_bus.write_data(0x12);

    // ensure that we aren't just reading what's latched onto the bus from the last write
    test_bus.set_address(0xFFFF);
    uint8_t result = test_bus.read_data();

    // read it back and verify this write worked correctly
    test_bus.set_address(OAMDATA);
    result = test_bus.read_data();

    REQUIRE(result == 0x12);
}

TEST_CASE_METHOD(emulator_test_fixtures, "ppu - Test mirroring", "[ppu]") {
    test_bus.set_address(OAMDATA); 
    test_bus.write_data(0x56);

    // OAMDATA is 0x2004, and with mirroring, reading/writing to 0x3444 should yield the same result, proving that we can read and write to any mirrored address
    test_bus.set_address(0x3444);
    uint8_t result = test_bus.read_data();
    CHECK(result == 0x56);

    // try again in the opposite direction
    test_bus.set_address(OAMDATA + (8 * 4)); // should wrap back to original value
    test_bus.write_data(0x13);

    // PPU CTRL is 0x2000, and with mirroring, reading/writing to 0x3450 should yield the same result, proving that we can read and write to any mirrored address
    test_bus.set_address(OAMDATA);
    result = test_bus.read_data();
    CHECK(result == 0x13);    
}

TEST_CASE_METHOD(emulator_test_fixtures, "ppu - Test write only port", "[ppu]") {
    test_bus.set_address(PPUCTRL);
    test_bus.write_data(0xFE);

    uint8_t result = test_bus.read_data();
    CHECK(result == 0);
}

TEST_CASE_METHOD(emulator_test_fixtures, "ppu - Test read only port", "[ppu]") {
    test_bus.set_address(PPUSTATUS);
    test_bus.write_data(0xFE);

    uint8_t result = test_bus.read_data();
    CHECK(result != 0xFE);
}

TEST_CASE_METHOD(emulator_test_fixtures, "ppu - Test palette ram registration", "[ppu]") {
    test_ppu_bus.set_address(PALETTE_RAM_INDEX_START);
    test_ppu_bus.write_data(0xFE);

    test_ppu_bus.set_address(0xFFFF);
    uint8_t result = test_ppu_bus.read_data();

    test_ppu_bus.set_address(PALETTE_RAM_INDEX_START);
    result = test_ppu_bus.read_data();

    CHECK(result == 0xFE);
}

TEST_CASE_METHOD(emulator_test_fixtures, "ppu - Test palette getter", "[ppu]") {
    /*for (uint8_t i = 0; i < 64; i++) {
        uint16_t R, G, B;
        R = test_ppu.NTSC_PALETTE[i][0];
        G = test_ppu.NTSC_PALETTE[i][1];
        B = test_ppu.NTSC_PALETTE[i][2];

        std::cout << "R: " << R << ", G: " << G << ", B: " << B << std::endl;
    }*/
    
    uint8_t red_value = test_ppu.NTSC_PALETTE[0x2C][0];
    REQUIRE(red_value == 56);
}

