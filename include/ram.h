#pragma once

#include "bus_device.h"

class ram : public bus_device
{
public:
	ram(bus* bus_ptr);
	~ram() = default;

	void set_read(void);
	void set_write(void);
	
private:
	uint8_t *ram_data;	// will use heap memory to store the 2Kb of memory used by the system.
};
