#include "emulator_status_graphics.h"

emulator_status_graphics::emulator_status_graphics(SDL_Renderer* renderer, const char* font_filename, int ptsize) {
    _renderer = renderer; 
    _font_size = ptsize;
    _x_pos = 0; 
    _y_pos = 0;

    if(!TTF_WasInit() && TTF_Init()== -1) {
        std::cout << "TTF_Init unsuccessful: " << TTF_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }

    _font = TTF_OpenFont(font_filename, _font_size);

    if (_font == NULL) {
		std::cout << "Error - font not found, error code: " << TTF_GetError() << std::endl;    	 
	    exit(EXIT_FAILURE);
    }    		

    set_colour({255, 255, 255, 255}); // Assuming a black background, text is white by default.
}


emulator_status_graphics::~emulator_status_graphics() {
	TTF_CloseFont(_font);
	SDL_FreeSurface(_text_surface);
	SDL_DestroyTexture(_text_texture);    
    TTF_Quit();
}

void emulator_status_graphics::set_colour(SDL_Color colour) {
    _colour = colour;
}

void emulator_status_graphics::set_position(uint16_t x_pos, uint16_t y_pos) {
    _x_pos = x_pos;
    _y_pos = y_pos;  
}

uint16_t emulator_status_graphics::get_latest_x_pos(void) {
    // this will only work after you have called draw_to_buffer at least once
    return _x_pos;       
}

uint16_t emulator_status_graphics::get_latest_y_pos(void) {
    // this will only work after you have called draw_to_buffer at least once
	return _y_pos; 
}

uint16_t emulator_status_graphics::get_latest_text_width(void) {
    // this will only work after you have called draw_to_buffer at least once
    return _text_texture_rect.w;       
}

uint16_t emulator_status_graphics::get_latest_text_height(void) {
    // this will only work after you have called draw_to_buffer at least once
	return _text_texture_rect.h; 
}

void emulator_status_graphics::draw_to_buffer(std::string text_to_render) {
    _text_surface = TTF_RenderText_Solid(_font, text_to_render.c_str(), _colour); // convert message into a texture
    _text_texture = SDL_CreateTextureFromSurface(_renderer, _text_surface);

	_text_texture_rect.x = _x_pos; 
	_text_texture_rect.y = _y_pos; 		
	_text_texture_rect.w = text_to_render.length() * _font_size; 
	_text_texture_rect.h = _font_size;  

    SDL_RenderCopy(_renderer, _text_texture, NULL, &_text_texture_rect); // copy any new textures to the renderer
}