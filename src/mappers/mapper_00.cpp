#include "mappers/mapper_00.h"

int mapper_00::cpu_read_address(uint16_t address) {
    // the mapper read address converts the absolute address into the relative index needed to read from ROM

    if (address >= PGM_ROM_ADDRESS_SPACE_START) { // && address <= PGM_ROM_ADDRESS_SPACE_END) { // removing for warnings
        uint16_t index = address - (PGM_ROM_ADDRESS_SPACE_END - _pgm_rom_size + 1);  // PGM rom always sits at the end of the memory map

        if (_pgm_rom_mirroring) {
            index = index % (PGM_ROM_SIZE_BYTES / 2);     // repeat every 16Kb for the 32Kb roms
        }
        return index;  
    }
    return -1;
}

void mapper_00::cpu_write_address(uint16_t address, uint8_t data) {
    data++;
    address++; // supress warnings. Do nothing
}

int mapper_00::ppu_read_address(uint16_t address) {
    // the mapper read address converts the absolute address into the relative index needed to read from ROM
    return address;  // TODO
}

void mapper_00::ppu_write_adress(uint16_t address, uint8_t data) {
    data++;
    address++; // supress warnings. Do nothing
}
