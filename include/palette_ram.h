#pragma once
#include "ram.h"

constexpr uint16_t SPECIAL_PALETTE_0 	= 0x3F00;
constexpr uint16_t SPECIAL_PALETTE_1 	= 0x3F04;
constexpr uint16_t SPECIAL_PALETTE_2 	= 0x3F08;
constexpr uint16_t SPECIAL_PALETTE_3 	= 0x3F0C;

constexpr uint16_t SPECIAL_MIRROR_0 	= 0x3F10;
constexpr uint16_t SPECIAL_MIRROR_1 	= 0x3F14;
constexpr uint16_t SPECIAL_MIRROR_2 	= 0x3F18;
constexpr uint16_t SPECIAL_MIRROR_3 	= 0x3F1C;

class palette_ram : public ram
{
public:
	palette_ram();
	~palette_ram() = default;

	uint8_t read(uint16_t address) override;
	void write(uint16_t address, uint8_t data) override;		
};
