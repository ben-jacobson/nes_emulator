#include "pattern_table_preview.h"

pattern_table_preview::pattern_table_preview(cartridge* cartridge_ptr, SDL_Renderer* renderer, uint16_t preset_display_x, uint16_t preset_display_y)
{
    // copy over our cartridge pointer
    _cartridge_ptr = cartridge_ptr;

    // set up our renderer
    _renderer = renderer;
    _preset_display_x = preset_display_x;
    _preset_display_y = preset_display_y;    

    // set up our rendering rectangle
    _rect.x = _preset_display_x;
    _rect.y = _preset_display_y;
    _rect.w = 8 * 16 * 2;    // each pattern is 8x8, show 16 columns of this, then x2 for one batch for left and one batch for right.
    _rect.h = 8 * 16;	

    // create the placeholder rectangle for when we eventually want to render some display
    _surface = SDL_CreateRGBSurface(0, _rect.w, _rect.h, 32, 0, 0, 0, 0); // 32 bit pixel depth

    // check that were able to successfully create our surface. 
    if (_surface == NULL) {
        std::cout << "CreateRGBSurface failed: " << SDL_GetError() << std::endl;
        exit(1);
    }

    _colour_depth = 4; 
    _pixel_data.resize(_rect.w * _rect.h * _colour_depth); 

    // fill our placeholder rectangle and create our texture, ready to be used again and again
    SDL_FillRect(_surface, NULL, SDL_MapRGB(_surface->format, 64, 64, 64)); // fill with grey for now
    _texture = SDL_CreateTextureFromSurface(_renderer, _surface);    	
}

pattern_table_preview::~pattern_table_preview() {
    SDL_FreeSurface(_surface);
    SDL_DestroyTexture(_texture);    
}

std::array<uint8_t, 8> pattern_table_preview::get_row_data(uint16_t address) {
    uint8_t row_data = _cartridge_ptr->ppu_read(address);
    std::array<uint8_t, 8> pixel_row;    

    // split out an 8 bit number into array values
    for (uint8_t i = 0; i < 8; i++) {
        pixel_row[i] = ((1 << i) & row_data) >> i;
    }

    return pixel_row; 
}

void pattern_table_preview::get_pattern(uint16_t address) {
    get_row_data(address);
}

void pattern_table_preview::clear_pixel_data(void) {
    // I've had it up to here with vector clearing!! There are like a dozen ways to clear them but most crash...
    // doing this the old fashioned way for now. 
    for (int i = 0; i < _rect.w * _rect.h * _colour_depth; i++) {
       _pixel_data[i] = 0; 
    }
}

void pattern_table_preview::display_contents(void) {
    clear_pixel_data();

    get_pattern(0x0000);

    // splat down some random pixels
    for(unsigned int i = 0; i < 1000; i++) {
        const unsigned int x = rand() % _rect.w;
        const unsigned int y = rand() % _rect.h;

        const unsigned int offset = ( _rect.w * 4 * y ) + x * 4;
        _pixel_data[ offset + 0 ] = rand() % 256;        // b
        _pixel_data[ offset + 1 ] = rand() % 256;        // g
        _pixel_data[ offset + 2 ] = rand() % 256;        // r
        _pixel_data[ offset + 3 ] = SDL_ALPHA_OPAQUE;    // a
    }

    SDL_UpdateTexture(_texture, NULL, _pixel_data.data(), _rect.w * 4);
    SDL_RenderCopy(_renderer, _texture, NULL, &_rect);
}

