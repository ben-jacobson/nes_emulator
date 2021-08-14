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

TEST_CASE_METHOD(emulator_test_fixtures, "ppu - Test set address port, write and read data", "[ppu]") {
   /* Remember, this is a process where the CPU interfaces with the PPU via it's ports. 
   // We'll start by simulating the CPU "instructing" the PPU to set it's address,
   // then read from the PPU bus to see if the address was set. A common use case for this is setting the palette RAM, like this:

        // set the ppu address bus to 0x3F00, via the exposed port 
        C03D   A9 3F                LDA #$3F
        C03F   8D 06 20             STA $2006
        C042   A9 00                LDA #$00
        C044   8D 06 20             STA $2006

        C047   A2 00                LDX #$00        // load 0 into x register
        C049   BD 78 FF   LC049     LDA $FF78,X     // load A with value in memory location FF78 = 0x0F
        C04C   8D 07 20             STA $2007       // write 0x0F data with port 2007
   */

    // First test that we can set our address
    test_ppu_bus.set_address(0x0000); // reset in case a previous test interferes

    test_bus.set_address(PPUADDR); 
    test_bus.write_data(0x3F);
    test_bus.set_address(PPUADDR); 
    test_bus.write_data(0x00);

    uint16_t result_addr = test_ppu_bus.read_address();
    CHECK(result_addr == 0x3F00);

    // Then test if we can write data and read it back via the PPU bus
    test_bus.set_address(PPUDATA);
    test_bus.write_data(0x1F);

    test_ppu_bus.set_address(PALETTE_RAM_INDEX_START);
    uint8_t result = test_ppu_bus.read_data();
    CHECK(result == 0x1F);

    // set the PPU address bus to something else just to make sure we aren't getting any false positives
    test_ppu_bus.set_address(0x0000);

    // Finally check if we can read data back from the port, reset the bus back and read the data
    test_bus.set_address(PPUADDR); 
    test_bus.write_data(0x3F);
    test_bus.set_address(PPUADDR); 
    test_bus.write_data(0x00);

    test_bus.set_address(PPUDATA);
    result = test_bus.read_data();
    CHECK(result == 0x1F);
}