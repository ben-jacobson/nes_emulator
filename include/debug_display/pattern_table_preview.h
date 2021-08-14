#pragma once

#include <SDL2/SDL.h>
#include <vector>

#include "bus.h"
#include "ppu.h"

constexpr uint8_t R = 0;
constexpr uint8_t G = 1;
constexpr uint8_t B = 2;

class pattern_table_preview
{
public:
	pattern_table_preview(bus* ppu_bus_ptr, ppu* ppu_ptr, SDL_Renderer* renderer, uint16_t preset_display_x, uint16_t preset_display_y);
	~pattern_table_preview();

	void get_pattern(uint16_t address);
	void convert_last_pattern_to_pixel_data(uint16_t x, uint16_t y);
	void display_contents(void);

	void select_palette(void);

private:
	void clear_pixel_data(void);

    uint16_t _preset_display_x, _preset_display_y; 	

	SDL_Renderer* _renderer;
	SDL_Surface* _surface; 
	SDL_Rect _rect;
	SDL_Texture* _texture;	

	bus* _ppu_bus_ptr;
	ppu* _ppu_ptr;

	// palette info
	uint8_t _palette_selected_index; 
	std::array<std::array<uint8_t, 3>, 4> _palette_selected; // four colours, RGB

    std::vector<uint8_t> _pixel_data;
	std::array<uint8_t, 8 * 8> _last_pattern_retrieved;
	uint8_t _colour_depth; 

	void update_palette_array(void);
};

