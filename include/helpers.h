#pragma once

#include <SDL2/SDL.h>
#include <iostream>
#include <vector>

//const std::vector<SDL_Keycode> HEX_KEYS = {SDLK_0, SDLK_1, SDLK_2, SDLK_3, SDLK_4, SDLK_5, SDLK_6, SDLK_7, SDLK_8, SDLK_9, SDLK_a, SDLK_b, SDLK_c, SDLK_d, SDLK_e, SDLK_f};
//bool hex_key(SDL_Keycode ksym); 

const std::vector<char> HEX_KEYS = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

bool hex_key(char character); 

class game_display_placeholder_output {
public:
	static constexpr uint16_t RECT_WIDTH = 256, RECT_HEIGHT = 240;	// matches resolution of the original nes. 

	game_display_placeholder_output(SDL_Renderer* renderer, uint16_t x_pos, uint16_t y_pos, uint8_t scale);
	~game_display_placeholder_output();
	void draw(void);

private:
	uint16_t _xpos, _ypos; 
	SDL_Renderer* _renderer;
	SDL_Surface* _surface; 
	SDL_Rect _rect;
	SDL_Texture* _texture;
};