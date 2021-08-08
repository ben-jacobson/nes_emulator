#include "helpers.h"

bool hex_key(char character) {
    for (auto& key : HEX_KEYS) {
        if (character == key)
            return true; 
	}
	return false;    
}

game_display_placeholder_output::game_display_placeholder_output(SDL_Renderer* renderer, uint16_t x_pos, uint16_t y_pos, uint8_t scale) {
    _renderer = renderer;

    _rect.x = x_pos;
    _rect.y = y_pos;
    _rect.w = RECT_WIDTH * scale; 
    _rect.h = RECT_HEIGHT * scale;		

    // create a placeholder rectangle for when we eventually want to render some display
    _surface = SDL_CreateRGBSurface(0, _rect.w, _rect.h, 32, 0, 0, 0, 0); // 32 bit pixel depth

    if (_surface == NULL) {
        std::cout << "CreateRGBSurface failed: " << SDL_GetError() << std::endl;
        exit(1);
    }

    SDL_FillRect(_surface, NULL, SDL_MapRGB(_surface->format, 64, 64, 64));
    _texture = SDL_CreateTextureFromSurface(_renderer, _surface);
}

game_display_placeholder_output::~game_display_placeholder_output() {
    SDL_FreeSurface(_surface);
    SDL_DestroyTexture(_texture);
}

void game_display_placeholder_output::draw(void) {
    SDL_RenderCopy(_renderer, _texture, NULL, &_rect);
}

memory_peek_text_input_processor::memory_peek_text_input_processor() {
    _char_count = 0;
}

uint16_t memory_peek_text_input_processor::process(void) {
    std::string padded_text_input = std::string(4 - text_input.length(), '0') + text_input;
    uint16_t byte_val = strtol(padded_text_input.c_str(), NULL, 16);
    
    //std::cout << "User entered: " << std::hex << byte_val << std::endl;
    text_input = std::string(); // clear the string
    _char_count = 0;		// reset out char count*/

    return byte_val;
}
 
bool memory_peek_text_input_processor::add_character(char input) {
	char key_pressed = toupper(input);
    
	if (hex_key(key_pressed)) {	 // only append if a hex key
        text_input += key_pressed;
        _char_count++;

        //std::cout << "Character: " << key_pressed << std::endl;

        if (_char_count >= 4) {
            return true; 
        }
    }
    return false;
}