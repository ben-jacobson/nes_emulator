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

	bool _latched_address, _instruction_ready; // has this come across as an addressed command? 
	uint16_t _latched_address_low, _latched_address_high;

	uint8_t _PPU_control_register, _PPU_mask_register, _PPU_status_register, _PPU_oam_addr_status_register, _PPU_oam_data_status_register, _PPU_data_register;
};
