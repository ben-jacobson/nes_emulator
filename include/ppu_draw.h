#pragma once

#include "ppu.h"

class ppu_draw
{
public:
	ppu_draw(ppu* ppu_ptr, SDL_Renderer* renderer, uint16_t preset_display_x, uint16_t preset_display_y, uint8_t pixel_scaling);
	~ppu_draw();
    void draw(void);

private:
    ppu* _ppu_ptr;

    uint16_t _preset_display_x, _preset_display_y; 	
    uint8_t _pixel_scaling; 

	SDL_Renderer* _renderer;
	SDL_Surface* _surface; 
	SDL_Rect _rect;
	SDL_Texture* _texture;	
};
