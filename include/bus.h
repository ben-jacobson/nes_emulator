#pragma once

#include <stdint.h>

class bus
{
	public:
		bus() = default;
		~bus() = default;
				
		void set_address(uint16_t address);
		uint16_t read_address(void);

		void write_data(uint8_t data);
		uint8_t read_data(void);
			
	private:
		uint16_t _address;
		uint8_t _data;
};