#pragma once

#include <iomanip>
#include <vector>

#include "bus_device.h"
#include "memory_map.h"

// we'll need to include every mapper that we plan to ever use
#include "mapper_base.h"
#include "mapper_00.h"

class cartridge : public bus_device
{
public:
	cartridge(uint16_t address_space_lower, uint16_t address_space_upper);
	~cartridge() = default;

	// functions for reading and writing  data to and from the bus
	uint8_t read_rom(uint16_t address); 

	// the default bus_device functions are used for CPU read/write, not for PPU, we'll have a separate one for that
	uint8_t read(uint16_t address) override;
	void write(uint16_t address, uint8_t data) override;	

	uint8_t ppu__read(uint16_t address);
	void ppu_write(uint16_t address, uint8_t data);	

	bool load_rom(std::string filename);
	void load_content_from_stream(std::string bytecode, uint16_t destination_address = 0);

	// for outputting to test functions, and our debug view
	uint8_t debug_read(uint16_t relative_address);	// for rom output view, not for actual use in emulation
	void debug_write(uint16_t relative_address, uint8_t data);

private:
	mapper_base* _mapper;	// we won't know what mapper to use until we've read some of the ROM

	std::vector<uint8_t> _pgm_rom_data;	// we won't know the size of these elements until we've read some of the ROM
	std::vector<uint8_t> _chr_rom_data;
};
