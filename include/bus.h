#pragma once

#include <stdint.h>

constexpr uint8_t MAX_BUS_DEVICES = 16; // just to keep our memory consumption down for this. 

struct connected_device {
	uint16_t _address_range_start, _address_range_end;
	uint8_t (*_read_fn_ptr);
};

class bus 
{
public:
	bus() = default;
	~bus() = default;
			
	void set_address(uint16_t address);
	uint16_t read_address(void);

	void write_data(uint8_t data);
	uint8_t read_data(void);

	void register_new_bus_device(uint16_t address_range_start, uint16_t address_range_end, uint8_t (*read_fn_ptr));
		
private:
	uint16_t _address;
	uint8_t _data;
	connected_device connected_devices[MAX_BUS_DEVICES];
};