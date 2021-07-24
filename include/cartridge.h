#pragma once

#include <iomanip>

#include "bus_device.h"
#include "memory_map.h"

class cartridge : public bus_device
{
public:
	cartridge(bus* bus_ptr, uint16_t address_space_lower, uint16_t address_space_upper);
	~cartridge();

	// functions for reading and writing  data to and from the bus
	uint8_t read_rom(uint16_t address); 
	uint8_t read(uint16_t address) override;
	void write(uint16_t address, uint8_t data) override;	
	void load_content_from_stream(std::string bytecode, uint16_t destination_address = 0);
	bool load_content_from_file(std::string filename, uint16_t start_address = 0);
	bool load_rom(std::string filename);

	// for outputting to test functions, and our debug view
	uint8_t debug_read(uint16_t relative_address);	// for rom output view, not for actual use in emulation
	uint8_t* get_rom_pointer(void);

private:
	uint8_t* _pgm_rom_data;	// will use heap memory to store the 32Kb of ROM used by the cartridge.
};
