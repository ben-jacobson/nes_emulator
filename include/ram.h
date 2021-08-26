#pragma once
#include "bus_device.h"
#include "memory_map.h"

class ram : public bus_device
{
public:
	ram(uint16_t ram_size, uint16_t address_space_lower, uint16_t address_space_upper);
	~ram();

	// for outputting to test functions, and our debug view
	uint8_t debug_read(uint16_t relative_address);	// for ram output view, not for actual use in emulation
	uint8_t* get_ram_pointer(void);
	
	// functions for reading and writing data to and from the device
	uint8_t read(uint16_t address) override;
	void write(uint16_t address, uint8_t data) override;	

	void clear_ram(void);

protected:
	uint16_t _ram_size;
	uint8_t* _ram_data;	// will use heap memory to store the 2Kb of memory used by the system.
};