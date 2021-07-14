#pragma once

#include <SDL2/SDL_ttf.h>
#include <iostream>

class emulator_status_graphics
{
	public:
		emulator_status_graphics(SDL_Renderer* renderer, const char* font_filename, int ptsize);
		~emulator_status_graphics();

		void set_font_width(uint8_t font_width);
		void set_font_height(uint8_t font_height);
		void set_colour(SDL_Color colour);
		void set_position(uint16_t x_pos, uint16_t y_pos);

		uint16_t get_x_pos(void);
		uint16_t get_y_pos(void);
		uint16_t get_text_width(std::string test);
		uint16_t get_text_height(std::string test);
		uint16_t get_last_rendered_text_width(void);
		uint16_t get_last_rendered_text_height(void);

		void draw_to_buffer(std::string text_to_render);
		void draw_to_buffer(std::string text_to_render, uint16_t x_pos, uint16_t y_pos);

	private:
		SDL_Renderer* _renderer; 			// Pointer to main renderer
		SDL_Surface* _text_surface; 		// Surface to paint text onto
		SDL_Texture* _text_texture;  		// The surface with text on it converted into a texture
		SDL_Rect _text_texture_rect;		// A rectangle blitting region containing the texture rendered onto a rectangle

	protected:
		int _font_size; 					// font point size
		uint8_t _font_width, _font_height;	// font width and height	
		uint16_t _x_pos, _y_pos; 			// x and y coords of text to be displayed
		TTF_Font* _font;					// font TTF file
		SDL_Color _colour; 					// text colour, can be changed at any time.
};
