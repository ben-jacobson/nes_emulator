#pragma once

#include "memory_map.h"
#include "bus.h"
#include "bus_device.h"

constexpr uint8_t A_KEY 		= 0;
constexpr uint8_t B_KEY 		= 1;
constexpr uint8_t SEL_KEY 		= 2;
constexpr uint8_t START_KEY 	= 3;
constexpr uint8_t UP_KEY 		= 4;
constexpr uint8_t DOWN_KEY 		= 5;
constexpr uint8_t LEFT_KEY 		= 6;
constexpr uint8_t RIGHT_KEY 	= 7;

constexpr uint8_t BUTTON_DOWN 	= 0;
constexpr uint8_t BUTTON_UP 	= 1;

constexpr uint8_t LOW 			= 0;
constexpr uint8_t HIGH 			= 1;

constexpr uint8_t PLAYER_ONE 	= 1;
constexpr uint8_t PLAYER_TWO 	= 2;

class apu_io : public bus_device
{
public:
	apu_io(uint16_t address_space_lower, uint16_t address_space_upper);
	~apu_io() = default;

	uint8_t read(uint16_t address) override;
	void write(uint16_t address, uint8_t data) override;

	void process_key(uint8_t player, uint8_t key, uint8_t state);

	void debug_set_key_buffer(uint8_t player, uint8_t value);
	uint8_t debug_get_key_buffer(uint8_t player);

	void debug_set_key_register(uint8_t player, uint8_t value);
	uint8_t debug_get_key_register(uint8_t player);

private:
	bus* _bus_ptr;

	uint8_t controller_buffer_one, controller_buffer_two; // store the data at these registers to here. 
	uint8_t controller_register_one, controller_register_two; // store the data at these registers to here when shifting out. 
	uint8_t _strobe_count; 
	bool _strobe_sequence;
};
