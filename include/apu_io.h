#pragma once

#include "bus.h"
#include "bus_device.h"

constexpr uint8_t UP_KEY 		= 0;
constexpr uint8_t DOWN_KEY 		= 1;
constexpr uint8_t LEFT_KEY 		= 2;
constexpr uint8_t RIGHT_KEY 	= 3;
constexpr uint8_t A_KEY 		= 4;
constexpr uint8_t B_KEY 		= 5;
constexpr uint8_t START_KEY 	= 6;
constexpr uint8_t SEL_KEY 		= 7;

constexpr uint8_t BUTTON_UP 	= 0;
constexpr uint8_t BUTTON_DOWN 	= 1;

class apu_io : public bus_device
{
public:
	apu_io(bus* bus_ptr, uint16_t address_space_lower, uint16_t address_space_upper);
	~apu_io() = default;

	uint8_t read(uint16_t address) override;
	void write(uint16_t address, uint8_t data) override;

	void process_key(uint8_t key, uint8_t state);

private:
	bus* _bus_ptr;
};
