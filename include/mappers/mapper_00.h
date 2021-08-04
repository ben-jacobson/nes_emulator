#pragma once

#include "mapper_base.h"

class mapper_00 : public mapper_base
{
public:
	mapper_00(uint16_t pgm_rom_size, uint16_t chr_rom_size);
	~mapper_00() = default;

	int cpu_read_address(uint16_t address) override;
	void cpu_write_address(uint16_t address, uint8_t data) override;

	int ppu_read_address(uint16_t address) override;
	void ppu_write_adress(uint16_t address, uint8_t data) override;    
};
