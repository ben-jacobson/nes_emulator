#pragma once

#include "bus_device.h"

class ram : public bus_device
{
public:
	ram(bus* bus_ptr, uint16_t ram_size, uint16_t address_space_lower, uint16_t address_space_upper);
	~ram();

	uint8_t debug_read(uint8_t address);	// for ram output view, not for actual use in emulation
	// functions for reading and writing RAM data to and from the bus
	void read(void);
	void write(void);
	
private:
	uint8_t* _ram_data;	// will use heap memory to store the 2Kb of memory used by the system.
};
