#pragma once
#include <stdint.h>

class bus
{
	public:
		bus();
		void set_address(uint16_t address);
		void write_data(uint8_t data);
		uint8_t read_data(void);
		~bus() = default;

	private:
		uint16_t _address;
		uint8_t _data;
};
