#include "palette_ram.h"

palette_ram::palette_ram()
    : ram(PALETTE_RAM_SIZE, PALETTE_RAM_INDEX_START, PALETTE_RAM_MIRROR_END)
{
    // nothing to do
}

uint8_t palette_ram::read(uint16_t address) {
    // First check if the read is within the specified address range
    if (address >= _address_space_lower && address <= _address_space_upper) {
        if (address == SPECIAL_PALETTE_0 || address == SPECIAL_MIRROR_0) {          // Nes has some strange special use cases, this wouldn't apply through normal mirroring
            address = SPECIAL_MIRROR_0;
        }
        if (address == SPECIAL_PALETTE_1 || address == SPECIAL_MIRROR_1) {
            address = SPECIAL_MIRROR_1;
        }
        if (address == SPECIAL_PALETTE_2 || address == SPECIAL_MIRROR_2) {
            address = SPECIAL_MIRROR_2;
        }
        if (address == SPECIAL_PALETTE_3 || address == SPECIAL_MIRROR_3) {
            address = SPECIAL_MIRROR_3;
        }     

        return _ram_data[(address - _address_space_lower) % _ram_size]; // new mapped address is offset by _address_space_lower;
    }
    return 0;
}

void palette_ram::write(uint16_t address, uint8_t data) {
    // First check if the read is within the specified address range
    if (address >= _address_space_lower && address <= _address_space_upper) {
        if (address == SPECIAL_PALETTE_0 || address == SPECIAL_MIRROR_0) {          // Nes has some strange special use cases, this wouldn't apply through normal mirroring
            address = SPECIAL_MIRROR_0;
        }
        if (address == SPECIAL_PALETTE_1 || address == SPECIAL_MIRROR_1) {
            address = SPECIAL_MIRROR_1;
        }
        if (address == SPECIAL_PALETTE_2 || address == SPECIAL_MIRROR_2) {
            address = SPECIAL_MIRROR_2;
        }
        if (address == SPECIAL_PALETTE_3 || address == SPECIAL_MIRROR_3) {
            address = SPECIAL_MIRROR_3;
        }     

        _ram_data[(address - _address_space_lower) % _ram_size] = data;  // place it into the array at the new offset address
    }
}