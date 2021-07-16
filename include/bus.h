#pragma once

#include <stdint.h>
#include <iostream>
#include <functional>

constexpr uint8_t MAX_BUS_DEVICES = 16; // just to keep our memory consumption down for this. 

struct connected_device {
	uint16_t _address_range_start, _address_range_end;
	std::function<uint8_t(uint16_t)> _read_function_ptr = nullptr;
	std::function<void(uint16_t, uint8_t)> _write_function_ptr = nullptr;  	
};

class bus 
{
public:
	bus();
	~bus() = default;
			
	void set_address(uint16_t address);
	uint16_t read_address(void);

	void write_data(uint8_t data);
	uint8_t read_data(void);
	void register_new_bus_device(uint16_t address_range_start, uint16_t address_range_end, std::function<uint8_t(uint16_t)> read_function_ptr, std::function<void(uint16_t, uint8_t)> write_function_ptr = nullptr);

private:
	uint16_t _address;
	uint8_t _data;
	connected_device devices_connected_to_bus[MAX_BUS_DEVICES];
	uint8_t device_index;

	int get_index_of_connected_device(uint16_t address);
};