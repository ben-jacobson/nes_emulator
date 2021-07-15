#pragma once
#include "bus_device.h"

class ram : public bus_device
{
public:
	ram(bus* bus_ptr, uint16_t ram_size, uint16_t address_space_lower, uint16_t address_space_upper);
	~ram();

	// for outputting to test functions, and our debug view
	uint8_t debug_read(uint16_t relative_address);	// for ram output view, not for actual use in emulation
	uint8_t* get_ram_pointer(void);
	
	// functions for reading and writing data to and from the device
	uint8_t read(uint16_t address);
	void write(uint16_t address, uint8_t data);

private:
	uint8_t* _ram_data;	// will use heap memory to store the 2Kb of memory used by the system.
};