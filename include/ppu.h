#pragma once

#include <stdint.h>

#include "bus.h"
#include "bus_device.h"
#include "cpu.h"

// the PPU is interfaced via a series of ports
constexpr uint16_t PPUCTRL 		= 0x2000;
constexpr uint16_t PPUMASK 		= 0x2001;
constexpr uint16_t PPUSTATUS 	= 0x2002;
constexpr uint16_t OAMADDR 		= 0x2003;
constexpr uint16_t OAMDATA 		= 0x2004;
constexpr uint16_t PPUSCROLL 	= 0x2005;
constexpr uint16_t PPUADDR 		= 0x2006;
constexpr uint16_t PPUDATA 		= 0x2007;
//constexpr uint16_t OAMDMA 		= 0x4014;

class ppu : public bus_device
{
public:
	ppu(bus* cpu_bus_ptr, bus* ppu_bus_ptr, cpu* cpu_ptr);
	~ppu() = default;

    void cycle(void);   // main PPU clocking
    void reset(void);   // reset PPU back to original state. 
	void trigger_cpu_NMI(void);

	// being a bus device, we need to define these, they refer to read and write activity on the main cpu bus
	uint8_t read(uint16_t address) override;
	void write(uint16_t address, uint8_t data) override;	   

    // For reading and writing to the PPU bus 
	//uint8_t PPUread(uint16_t address);
	//void PPUwrite(uint16_t address, uint8_t data);	   

private:
	//std::array <uint16_t, 9> ADDRESS_PORTS = {PPUCTRL, PPUMASK, PPUSTATUS, OAMADDR, OAMDATA, PPUSCROLL, PPUADDR, PPUDATA, OAMDMA};

	bus* _cpu_bus_ptr; 
	bus* _ppu_bus_ptr;
	cpu* _cpu_ptr; 

	uint8_t _PPU_control_register; 
	uint8_t _PPU_mask_register; 
	uint8_t _PPU_status_register; 
	uint8_t _PPU_oam_addr_status_register;
	uint8_t _PPU_oam_data_status_register;
	uint8_t _PPU_data_register;

	// our NTSC Pallete is stored internally
	std::array <std::array<uint8_t, 3>, 64> NTSC_PALLETTE = {{
		{84, 84, 84},
		{0, 30, 116},
		{8, 16, 144},
		{48, 0, 136},
		{68, 0, 100},
		{92, 0, 48},
		{84, 4, 0},
		{60, 24, 0},
		{32, 42, 0},
		{8, 58, 0},
		{0, 64, 0},
		{0, 60, 0},
		{0, 50, 60},
		{0, 0, 0},
		{0, 0, 0},
		{0, 0, 0},
		{152, 150, 152},
		{8, 76, 196},
		{48, 50, 236},
		{92, 30, 228},
		{136, 20, 176},
		{160, 20, 100},
		{152, 34, 32},
		{120, 60, 0},
		{84, 90, 0},
		{40, 114, 0},
		{8, 124, 0},
		{0, 118, 40},
		{0, 102, 120},
		{0, 0, 0},
		{0, 0, 0},
		{0, 0, 0},
		{236, 238, 236},
		{76, 154, 236},
		{120, 124, 236},
		{176, 98, 236},
		{228, 84, 236},
		{236, 88, 180},
		{236, 106, 100},
		{212, 136, 32},
		{160, 170, 0},
		{116, 196, 0},
		{76, 208, 32},
		{56, 204, 108},
		{56, 180, 204},
		{60, 60, 60},
		{0, 0, 0},
		{0, 0, 0},
		{236, 238, 236},
		{168, 204, 236},
		{188, 188, 236},
		{212, 178, 236},
		{236, 174, 236},
		{236, 174, 212},
		{236, 180, 176},
		{228, 196, 144},
		{204, 210, 120},
		{180, 222, 120},
		{168, 226, 144},
		{152, 226, 180},
		{160, 214, 228},
		{160, 162, 160},
		{0, 0, 0},
		{0, 0, 0}
	}}; 
};
