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
constexpr uint16_t OAMDMA 		= 0x4014;

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
constexpr uint8_t PPUMASK_SHOWLEFT_BG				= 1; 
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
constexpr uint8_t ATTRTABLE_WIDTH					= 8;
constexpr uint8_t ATTRTABLE_HEIGHT					= 8;	
constexpr uint8_t SPRITE_WIDTH						= 8;		// tile height is variable, 8 or 16 px and is stored as a member variable

constexpr uint8_t BACKGROUND_PALETTES				= 16;
constexpr uint8_t FOREGROUND_PALETTES				= 16;

constexpr uint8_t R = 0;
constexpr uint8_t G = 1;
constexpr uint8_t B = 2;

union loopy_register {
	// this is a copy of loopys register, https://wiki.nesdev.com/w/index.php/PPU_scrolling
	struct {

		uint16_t coarse_x : 5;
		uint16_t coarse_y : 5;
		uint16_t nametable_x : 1;
		uint16_t nametable_y : 1;
		uint16_t fine_y : 3;
		uint16_t unused : 1;
	};
	uint16_t reg = 0x0000;
};

struct oam_entry {
	// note the unusual order, this needs to match the order in which the OAM read and write need to see this. We won't always be accessing this as a structure, 
	// we'll more likely be accessing this using a pointer and an index, and incrementing through 8 bit memory addresses, so the order in which these are defined
	// is important
	uint8_t y; 			// y pos of sprite
	uint8_t id; 		// tile ID
	uint8_t attr; 		// various flags defining how this should be rendered
	uint8_t x; 			// x pos of sprite	
};

class ppu : public bus_device
{
public:
	ppu(bus* cpu_bus_ptr, bus* ppu_bus_ptr, cpu* cpu_ptr);
	~ppu() = default;

    void cycle(void);   // main PPU clocking
    void reset(void);   // reset PPU back to original state. 
	void trigger_cpu_NMI(void);
	void handle_dma(void);

	// being a bus device, we need to define these, they refer to read and write activity on the main cpu bus
	uint8_t read(uint16_t address) override;
	void write(uint16_t address, uint8_t data) override;	   

	void vertical_blank(void);
	uint16_t get_video_memory_address(void);
	//bool get_address_latch(void);
	bool get_vertical_blank(void);

	oam_entry debug_read_oam(uint8_t relative_address);
	uint8_t debug_read_oam_ptr(uint8_t relative_address);

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

	uint8_t _PPU_control_register; 
	uint8_t _PPU_mask_register; 
	uint8_t _PPU_status_register; 
	// uint8_t _PPU_data_register;
	uint8_t _buffered_read;	// PPU delays the read by one cycle unless reading from palette memory.

	loopy_register _temp_vram_address, _current_vram_address; // this is a bit like the CPU's program counter
	uint8_t _write_toggle, _fine_x_scroll;

	// Object Attribute Memory items
	uint8_t _oam_addr;	// we only need an 8 bit addres for this, seeing as the oam memory only ranges from 0-255
	oam_entry _oam_data[64]; // our total OAM data store is 256 bytes, the structure has 4 bytes in it, 64 * 4 = 256
	uint8_t* _ptr_oam_data = (uint8_t* )_oam_data;		// using a pointer, we can access this serially, rather than needing to use the structure addresses. 
	uint8_t _dma_page, _dma_addr, _dma_data; 	// and we can't have OAM without some DMA!
	bool _dma_transfer_status, _dma_requested;

	// ppu clock and scanline
	uint32_t _ppu_system_clock, _ppu_clock_at_start_of_dma;
	int _scanline_y;
	uint16_t _clock_pulse_x;
	uint32_t _frame_count; 
	bool _frame_complete_flag;

	// details about the colour depth
	uint8_t _colour_depth; 
	uint8_t _sprite_height; 
	
	// variables and array caches for the PPU helper functions
	uint8_t _result_pixel;
	uint16_t _nametable_x, _nametable_y;

	std::array <uint8_t, BACKGROUND_PALETTES> _background_palette_cache;
	std::array <uint8_t, FOREGROUND_PALETTES> _foreground_palette_cache;
	std::array <uint8_t, NAMETABLE_WIDTH> _nametable_row_cache;
	std::array <uint8_t, ATTRTABLE_WIDTH> _attribute_table_row_cache;
	std::array <std::array<uint8_t, SPRITE_WIDTH>, NAMETABLE_WIDTH> _pattern_row_plane_0_cache, _pattern_row_plane_1_cache;

	void increment_video_memory_address(void);
	void increment_scroll_x(void);
	void increment_scroll_y(void);

	void bg_load_shifters(void);
	void bg_update_shifters(void);

	// ppu helper functions
	void bg_set_pixel(void);
	bool bg_left_eight_pixels_enabled(void);

	bool bg_rendering_enabled(void);
	bool fg_rendering_enabled(void);

	void cache_nametable_row(void);
    void cache_pattern_row(void);
	void cache_attribute_table_row(void);
	void cache_bg_palettes(void);

	uint8_t _bg_next_tile_id,_bg_next_tile_attrib, _bg_next_tile_lsb, _bg_next_tile_msb;
	uint16_t _bg_shifter_pattern_lo, _bg_shifter_pattern_hi, _bg_shifter_attrib_lo, _bg_shifter_attrib_hi;
};
