#include "ppu_draw.h"

ppu_draw::ppu_draw(ppu* ppu_ptr, SDL_Renderer* renderer, uint16_t preset_display_x, uint16_t preset_display_y, uint8_t pixel_scaling) {
    // copy the pointer across
    _ppu_ptr = ppu_ptr;

    // set up our renderer and display stuff
    _renderer = renderer;
    _preset_display_x = preset_display_x;
    _preset_display_y = preset_display_y;   
    _pixel_scaling = pixel_scaling;

    _rect.x = _preset_display_x;
    _rect.y = _preset_display_y;
    _rect.w = FRAME_WIDTH;    
    _rect.h = FRAME_HEIGHT;	    

    // create the placeholder rectangle for when we eventually want to render some display
    _surface = SDL_CreateRGBSurface(0, _rect.w, _rect.h, 8, 0, 0, 0, 0); // 8 bit pixel depth, which is more than enough for what we need

    // check that were able to successfully create our surface. 
    if (_surface == NULL) {
        std::cout << "CreateRGBSurface failed: " << SDL_GetError() << std::endl;
        exit(1);
    }

    // fill our placeholder rectangle and create our texture, ready to be used again and again
    SDL_FillRect(_surface, NULL, SDL_MapRGB(_surface->format, 64, 64, 64)); // fill with grey for now
    _texture = SDL_CreateTextureFromSurface(_renderer, _surface);    	    
}

ppu_draw::~ppu_draw() {
    SDL_FreeSurface(_surface);
    SDL_DestroyTexture(_texture);   
}

void ppu_draw::draw(void) {
    SDL_UpdateTexture(_texture, NULL, _ppu_ptr->_raw_pixel_data.data(), _rect.w * 4);
    SDL_RenderCopy(_renderer, _texture, NULL, &_rect);
}