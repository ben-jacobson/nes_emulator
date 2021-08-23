#pragma once

#include <string>
#include <iostream>
#include <SDL2/SDL_ttf.h>

class text_renderer
{
public:
	text_renderer(std::string font_fullpath, uint8_t font_size, SDL_Renderer* renderer);
	~text_renderer();

	void draw_text(std::string text, uint16_t x_pos, uint16_t y_pos);
	void draw_char(char c, uint16_t x_pos, uint16_t y_pos);
	void debug_draw_atlas(uint16_t x_pos, uint16_t y_pos);
	void set_colour(SDL_Color colour);

	uint8_t _font_size; 					// font point size

private:
	SDL_Renderer* _renderer; 			// Pointer to main renderer
	SDL_Surface* _surface; 				// Surface to paint text onto
	SDL_Texture* _atlas_texture;  		// The texture containing entire font atlas
	//SDL_Texture* _text_texture;			// A texture set as a streaming target, updated for each character draw
	SDL_Rect _atlas_rect;				// A rectangle bounding the atlas texture, resizable depending on what's being rendered

	TTF_Font* _font;					// font TTF file
	SDL_Color _colour; 					// text colour, can be changed at any time.

	void update_atlas(void);
};
