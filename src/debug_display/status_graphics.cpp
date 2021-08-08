#include "status_graphics.h"

status_graphics::status_graphics(SDL_Renderer* renderer, const char* font_filename, int ptsize) {
    _renderer = renderer; 
    _font_size = ptsize;
    _x_pos = 0; 
    _y_pos = 0;

    if(!TTF_WasInit() && TTF_Init() != 0) {
        std::cout << "TTF_Init unsuccessful: " << TTF_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }

    _font = TTF_OpenFont(font_filename, _font_size);

    if (_font == NULL) {
        std::cout << "Error - font not found, error code: " << TTF_GetError() << std::endl;    	 
        exit(EXIT_FAILURE);
    }   

    set_colour({255, 255, 255, 255}); // Assuming a black background, text is white by default.
    set_font_width(_font_size);
    set_font_height(_font_size); // if not specified, simply make the font square to the font_size
}

void status_graphics::set_font_width(uint8_t font_width) {
    _font_width = font_width;
}

void status_graphics::set_font_height(uint8_t font_height) {
    _font_height = font_height;
}

status_graphics::~status_graphics() {
    if (TTF_WasInit()) {  // prevent the handler from deleting ths font twice
	    TTF_CloseFont(_font);
        TTF_Quit();
    } 
}

void status_graphics::set_colour(SDL_Color colour) {
    _colour = colour;
}

void status_graphics::set_position(uint16_t x_pos, uint16_t y_pos) {
    _x_pos = x_pos;
    _y_pos = y_pos;  
}

uint16_t status_graphics::get_x_pos(void) {
    // this will only work after you have called draw_to_buffer at least once
    return _x_pos;       
}

uint16_t status_graphics::get_y_pos(void) {
    // this will only work after you have called draw_to_buffer at least once
	return _y_pos; 
}

uint16_t status_graphics::get_text_width(std::string text) {
    int width = 0; 
    TTF_SizeText(_font, text.c_str(), &width, NULL);
    return (uint16_t)width; 
}   

uint16_t status_graphics::get_text_height(std::string text) {
    int height = 0; 
    TTF_SizeText(_font, text.c_str(), NULL, &height);
    return (uint16_t)height; 
}

uint16_t status_graphics::get_last_rendered_text_width(void) {
    // this will only work after you have called draw_to_buffer at least once
    return _text_texture_rect.w;       
}

uint16_t status_graphics::get_last_rendered_text_height(void) {
    // this will only work after you have called draw_to_buffer at least once
	return _text_texture_rect.h; 
}

void status_graphics::draw_to_buffer(std::string text_to_render) {
    _text_texture_rect.x = _x_pos; 
	_text_texture_rect.y = _y_pos; 		
	_text_texture_rect.w = text_to_render.length() * _font_width; 
	_text_texture_rect.h = _font_height;  

    _text_surface = TTF_RenderText_Solid(_font, text_to_render.c_str(), _colour); // convert message into a texture, just once
    _text_texture = SDL_CreateTextureFromSurface(_renderer, _text_surface); 
    
    SDL_RenderCopy(_renderer, _text_texture, NULL, &_text_texture_rect); // copy any new textures to the renderer

    SDL_DestroyTexture(_text_texture);
    SDL_FreeSurface(_text_surface);
}

void status_graphics::draw_to_buffer(std::string text_to_render, uint16_t x_pos, uint16_t y_pos) {
    set_position(x_pos, y_pos);
    draw_to_buffer(text_to_render);
}
