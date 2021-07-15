#pragma once

#include "bus_device.h"
#include "memory_map.h"

class cartridge : public bus_device
{
public:
	cartridge(bus* bus_ptr, uint16_t address_space_lower, uint16_t address_space_upper);
	~cartridge();

	// functions for reading and writing  data to and from the bus
	void read_rom(void);

	// for outputting to test functions, and our debug view
	uint8_t debug_read(uint16_t relative_address);	// for rom output view, not for actual use in emulation
	uint8_t* get_rom_pointer(void);
	
private:
	uint8_t* _rom_data;	// will use heap memory to store the 32Kb of ROM used by the cartridge.
};
