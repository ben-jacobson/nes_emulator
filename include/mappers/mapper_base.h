#pragma once

#include <stdint.h>

#include "memory_map.h"

class mapper_base
{
public:
	mapper_base() = default;
	~mapper_base() = default;

	virtual int cpu_read_address(uint16_t address) = 0;
	virtual void cpu_write_address(uint16_t address, uint8_t data) = 0;

	virtual int ppu_read_address(uint16_t address) = 0;
	virtual void ppu_write_adress(uint16_t address, uint8_t data) = 0;
	
	void set_pgm_rom_mirroring(bool value);
	bool get_pgm_rom_mirroring(void);

	void set_pgm_rom_size(uint16_t size);
	void set_chr_rom_size(uint16_t size);

protected:
	bool _pgm_rom_mirroring; 
	uint16_t _pgm_rom_size, _chr_rom_size; 
};
