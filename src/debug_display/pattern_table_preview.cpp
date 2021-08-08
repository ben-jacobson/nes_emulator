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
    _surface = SDL_CreateRGBSurface(0, _rect.w, _rect.h, 8, 0, 0, 0, 0); // 8 bit pixel depth, which is more than enough for what we need

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

void pattern_table_preview::get_pattern(uint16_t address) {
    uint8_t row_data_plane_0 = _cartridge_ptr->ppu_read(address);
    uint8_t row_data_plane_1 = _cartridge_ptr->ppu_read(address + 1);

    // read the pattern, row by row
    for (uint8_t i = 0; i < 8; i++) {        
        row_data_plane_0 = _cartridge_ptr->ppu_read(address + i);
        row_data_plane_1 = _cartridge_ptr->ppu_read(address + i + 8);      
        uint8_t plane_0_bit = 0, plane_1_bit = 0;

        // split out an 8 bit value into array of values. Oring them to get the full value from 0-4        
        for (uint8_t j = 0; j < 8; j++) {
            plane_0_bit = ((1 << j) & row_data_plane_0) >> j;
            plane_1_bit = ((1 << j) & row_data_plane_1) >> j;
            _last_pattern_retrieved[(i * 8) + (8 - j - 1)] = plane_0_bit | (plane_1_bit << 1);  // place them in MSB order
        }
    }   
}

void pattern_table_preview::clear_pixel_data(void) {
    // I've had it up to here with vector clearing!! There are like a dozen ways to clear them but most crash...
    // doing this the old fashioned way for now. 
    for (int i = 0; i < _rect.w * _rect.h * _colour_depth; i++) {
       _pixel_data[i] = 0; 
    }
}

void pattern_table_preview::convert_last_pattern_to_pixel_data(uint16_t x, uint16_t y) {
    std::array <uint8_t, 4> R_values = {0, 64, 128, 255};       // placeholder, grayscale for now and will be replaced with actual pallettes
    std::array <uint8_t, 4> G_values = {0, 64, 128, 255};
    std::array <uint8_t, 4> B_values = {0, 64 ,128, 255};
    uint16_t new_x = x, new_y = y; 
    unsigned int offset;

    for (uint8_t i = 0; i < 8 * 8; i++) {
        // convert to pixel offset value
        offset = ( _rect.w * 4 * new_y ) + (new_x * 4);

        // fill in the RGB values
        if (offset + 3 < _pixel_data.size()) {
            _last_pattern_retrieved[i] = _last_pattern_retrieved[i] & 0x03; // lop off the top 6 bits in case we try to read something outside of the valid memory range.
            _pixel_data[offset + 0] = R_values[_last_pattern_retrieved[i]];
            _pixel_data[offset + 1] = G_values[_last_pattern_retrieved[i]];
            _pixel_data[offset + 2] = B_values[_last_pattern_retrieved[i]];
            _pixel_data[offset + 3] = SDL_ALPHA_OPAQUE;
        }
        new_x++;

        if(new_x >= x + 8) {
            new_x = x; 
            new_y++;
        }
    }
}

void pattern_table_preview::display_contents(void) {
    clear_pixel_data();
    uint8_t x = 0, y = 0; 

    // render pattern table left
    for (uint16_t i = 0; i < 512; i += 2) {
        get_pattern(i * 8);
        convert_last_pattern_to_pixel_data(x * 8, y * 8); 
        x++;

        if (x >= 16) {
            x = 0;
            y++;
        }
    }
    
    x = 16, y = 0; // reset the grid

    // render pattern table right
    for (uint16_t i = 512; i < 1024; i += 2) {
        get_pattern(i * 8);
        convert_last_pattern_to_pixel_data(x * 8, y * 8); 
        x++;

        if (x >= 32) {
            x = 16;
            y++;
        }
    }  

    SDL_UpdateTexture(_texture, NULL, _pixel_data.data(), _rect.w * 4);
    SDL_RenderCopy(_renderer, _texture, NULL, &_rect);
}

