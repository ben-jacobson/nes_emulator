#include "mapper_base.h"

void mapper_base::set_pgm_rom_mirroring(bool value) {
    _pgm_rom_mirroring = value;
}

void mapper_base::set_pgm_rom_size(uint16_t size) {
    _pgm_rom_size = size; 
}

void mapper_base::set_chr_rom_size(uint16_t size) {
    _chr_rom_size = size; 
}