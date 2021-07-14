#pragma once

#include <SDL2/SDL_ttf.h>
#include <iostream>

class emulator_status_graphics
{
	public:
		emulator_status_graphics(SDL_Renderer* renderer, const char* font_filename, int ptsize);
		~emulator_status_graphics();

		void set_colour(SDL_Color colour);
		void set_position(uint16_t x_pos, uint16_t y_pos);
		void draw_to_buffer(std::string text_to_render);

		uint16_t get_latest_x_pos(void);
		uint16_t get_latest_y_pos(void);
		uint16_t get_latest_text_width(void);
		uint16_t get_latest_text_height(void);

	private:
		SDL_Surface* _text_surface; 	// Surface to paint text onto
		SDL_Texture* _text_texture;  	// The surface with text on it converted into a texture
		SDL_Rect _text_texture_rect;	// A rectangle blitting region containing the texture rendered onto a rectangle
		SDL_Renderer* _renderer; 		// Pointer to main renderer

	    uint16_t _x_pos, _y_pos; 		// x and y coords of text to be displayed
		TTF_Font* _font;				// font TTF ffile
		int _font_size; 				// font point size
		SDL_Color _colour; 				// text colour, can be changed at any time.
};
