#pragma once

#include <stdint.h>

#include "bus.h"
#include "bus_device.h"

// the PPU is interfaced via a series of ports
constexpr uint16_t PPUCTRL 		= 0x2000;
constexpr uint16_t PPUMASK 		= 0x2001;
constexpr uint16_t PPUSTATUS 	= 0x2002;
constexpr uint16_t OAMADDR 		= 0x2003;
constexpr uint16_t OAMDATA 		= 0x2004;
constexpr uint16_t PPUSCROLL 	= 0x2005;
constexpr uint16_t PPUADDR 		= 0x2006;
constexpr uint16_t PPUDATA 		= 0x2007;
constexpr uint16_t OAMDMA 		= 0x4014;

class ppu : public bus_device
{
public:
	ppu(bus* cpu_bus_ptr, bus* ppu_bus_ptr);
	~ppu() = default;

	// being a bus device, we need to define these, they refer to read and write activity on the main cpu bus
	uint8_t read(uint16_t address) override;
	void write(uint16_t address, uint8_t data) override;	   

    // For reading and writing to the PPU bus 
	uint8_t PPUread(uint16_t address);
	void PPUwrite(uint16_t address, uint8_t data);	   

private:
	bus* _cpu_bus_ptr; 
	bus* _ppu_bus_ptr;
};
