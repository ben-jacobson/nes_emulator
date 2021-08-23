#pragma once

#include <stdint.h>
#include <vector>

#include <SDL2/SDL.h>

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
//constexpr uint16_t OAMDMA 		= 0x4014;		// this will need to be implemented elsewhere, maybe the APU/IO?? unsure. will come back to this later

// PPUCTRL flag bits
constexpr uint8_t PPUCTRL_NAME_TABLE_ADDRESS_LO 	= 0;
constexpr uint8_t PPUCTRL_NAME_TABLE_ADDRESS_HI 	= 1;
constexpr uint8_t PPUCTRL_VRAM_INCREMENT 			= 2;
constexpr uint8_t PPUCTRL_FG_PATTERN_TABLE_ADDR 	= 3;			// for sprites
constexpr uint8_t PPUCTRL_BG_PATTERN_TABLE_ADDR 	= 4;			// for backgrounds
constexpr uint8_t PPUCTRL_SPRITE_SIZE		 		= 5;	
constexpr uint8_t PPUCTRL_PPU_MASTER_SLAVE		 	= 6;	
constexpr uint8_t PPUCTRL_VERTICAL_BLANK_NMI		= 7; 

// PPUSTATUS flag bits first 5 bits are the LSB of written to PPU register
constexpr uint8_t PPUSTATUS_SPRITE_OVERFLOW			= 5; 
constexpr uint8_t PPUSTATUS_SPRITE_ZERO_HIT			= 6; 
constexpr uint8_t PPUSTATUS_VERTICAL_BLANK			= 7; 

// PPUMASK flag bits
constexpr uint8_t PPUMASK_SHOWLEFT_BACK				= 1; 
constexpr uint8_t PPUMASK_SHOWLEFT_SPRITES			= 2; 
constexpr uint8_t PPUMASK_SHOW_BACKGROUND			= 3; 
constexpr uint8_t PPUMASK_SHOW_SPRITES				= 4; 
constexpr uint8_t PPUMASK_EMPHASISE_RED				= 5; 
constexpr uint8_t PPUMASK_EMPHASISE_GREEN			= 6; 
constexpr uint8_t PPUMASK_EMPHASISE_BLUE			= 7; 

// details about rendering surface
constexpr uint16_t FRAME_WIDTH						= 256;
constexpr uint16_t FRAME_HEIGHT						= 240;
constexpr uint32_t FRAME_ARRAY_SIZE					= FRAME_WIDTH * FRAME_HEIGHT;	// this can fit into a 16 bit int, but just for safety in case our scaling screws this up
constexpr uint16_t PIXELS_PER_SCANLINE				= 341;
constexpr uint16_t SCANLINES_PER_FRAME				= 262;
constexpr uint16_t VISIBLE_SCANLINES				= FRAME_HEIGHT;

constexpr uint8_t NAMETABLE_WIDTH					= 32;
constexpr uint8_t NAMETABLE_HEIGHT					= 30;	
constexpr uint8_t SPRITE_HEIGHT						= 8;		// tile width is variable, 8 or 16 px and is stored as a member variable

constexpr uint8_t R = 0;
constexpr uint8_t G = 1;
constexpr uint8_t B = 2;

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

	void vertical_blank(void);
	uint16_t get_video_memory_address(void);
	bool get_address_latch(void);
	bool get_vertical_blank(void);

	uint16_t get_clock_pulses(void);
	uint16_t get_x(void);
	int get_y(void);
	uint32_t get_frame_count(void);
	bool get_frame_complete_flag(void);
	void clear_frame_complete_flag(void);

	// our NTSC Palette is stored internally to the device, at some point in the future it might be good to load a .pal file for this. 
	const std::array <std::array<uint8_t, 3>, 64> NTSC_PALETTE = {{	
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

    std::vector<uint8_t> _raw_pixel_data;

private:
	//std::array <uint16_t, 9> ADDRESS_PORTS = {PPUCTRL, PPUMASK, PPUSTATUS, OAMADDR, OAMDATA, PPUSCROLL, PPUADDR, PPUDATA, OAMDMA};

	bus* _cpu_bus_ptr; 
	bus* _ppu_bus_ptr;
	cpu* _cpu_ptr; 

	uint16_t _video_memory_address; // this is a bit like the CPU's program counter

	uint8_t _PPU_control_register; 
	uint8_t _PPU_mask_register; 
	uint8_t _PPU_status_register; 
	uint8_t _PPU_oam_addr_status_register;
	uint8_t _PPU_oam_data_status_register;
	uint8_t _PPU_data_register;
	uint8_t _ppu_addr_temp_register;

	bool _address_latch, _addr_second_write, _scroll_second_write;

	void increment_video_memory_address(void);

	int _scanline_y;
	uint16_t _clock_pulse_x;

	uint8_t _colour_depth; 
	uint8_t _sprite_width; 

	uint32_t _frame_count; 
	bool _frame_complete_flag;
};
