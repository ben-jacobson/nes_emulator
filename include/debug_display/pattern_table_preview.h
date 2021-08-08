#pragma once

#include <SDL2/SDL.h>
#include "cartridge.h"

class pattern_table_preview
{
public:
	pattern_table_preview(cartridge* cartridge_ptr, SDL_Renderer* renderer, uint16_t preset_display_x, uint16_t preset_display_y);
	~pattern_table_preview();

	std::array<uint8_t, 8> get_row_data(uint16_t address);
	void get_pattern(uint16_t address);

	void display_contents(void);

private:
	void clear_pixel_data(void);

    uint16_t _preset_display_x, _preset_display_y; 	

	SDL_Renderer* _renderer;
	SDL_Surface* _surface; 
	SDL_Rect _rect;
	SDL_Texture* _texture;	

	cartridge* _cartridge_ptr;

    std::vector<uint8_t> _pixel_data;
	std::array<uint8_t, 8 * 8> _last_pattern_retrieved;
	uint8_t _colour_depth; 
};
