#include "pattern_table_preview.h"

pattern_table_preview::pattern_table_preview(bus* ppu_bus_ptr, ppu* ppu_ptr, SDL_Renderer* renderer, uint16_t preset_display_x, uint16_t preset_display_y)
{
    // set up the ppu bus ptr
    _ppu_bus_ptr = ppu_bus_ptr;
    _ppu_ptr = ppu_ptr;

    _palette_selected_index = 0; // offset 0, viewing palettes from memory 0x3F00
    update_palette_array(); // feed in initial palette data, should be RGB 0,0,0

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
    // read the pattern, row by row
    for (uint8_t i = 0; i < 8; i++) {     
        uint8_t row_data_plane_0, row_data_plane_1, plane_0_bit = 0, plane_1_bit = 0;

        if (address + i + 8 <= PATTERN_TABLE_1_END) {
            _ppu_bus_ptr->set_address(address + i);
            row_data_plane_0 = _ppu_bus_ptr->read_data();
            _ppu_bus_ptr->set_address(address + i + 8);
            row_data_plane_1 = _ppu_bus_ptr->read_data();              
        }
        
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
    /*std::array <uint8_t, 4> R_values = {0, 64, 128, 255};       // placeholder, grayscale for now and will be replaced with actual pallettes
    std::array <uint8_t, 4> G_values = {0, 64, 128, 255};
    std::array <uint8_t, 4> B_values = {0, 64 ,128, 255};*/
    uint16_t new_x = x, new_y = y; 
    unsigned int offset;

    for (uint8_t i = 0; i < 8 * 8; i++) {
        // convert to pixel offset value
        offset = ( _rect.w * 4 * new_y ) + (new_x * 4);

        // fill in the RGB values
        if (offset + 3 < _pixel_data.size()) {
            _last_pattern_retrieved[i] = _last_pattern_retrieved[i] & 0x03; // lop off the top 6 bits in case we try to read something outside of the valid memory range.
           /* _pixel_data[offset + 0] = R_values[_last_pattern_retrieved[i]];
            _pixel_data[offset + 1] = G_values[_last_pattern_retrieved[i]];
            _pixel_data[offset + 2] = B_values[_last_pattern_retrieved[i]];*/

            _pixel_data[offset + 0] = _palette_selected[_last_pattern_retrieved[i]][R];
            _pixel_data[offset + 1] = _palette_selected[_last_pattern_retrieved[i]][G];
            _pixel_data[offset + 2] = _palette_selected[_last_pattern_retrieved[i]][B];            
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

void pattern_table_preview::update_palette_array(void) {
    // grab the four colours associated with this pallette
    uint16_t abs_address = 0x3F00 + _palette_selected_index;
    //std::cout << "Switching to palette: " << std::hex << abs_address << std::dec << std::endl;

    for (uint8_t i = 0; i < 4; i++) {
        _ppu_bus_ptr->set_address(abs_address + i);
        uint8_t colour_index = _ppu_bus_ptr->read_data();

        _palette_selected[i][R] = _ppu_ptr->NTSC_PALETTE[colour_index][R];
        _palette_selected[i][G] = _ppu_ptr->NTSC_PALETTE[colour_index][G];
        _palette_selected[i][B] = _ppu_ptr->NTSC_PALETTE[colour_index][B];
        //std::cout << "  index: 0x" << std::hex << (uint16_t)colour_index << std::dec << " -  R: " << (uint16_t)_palette_selected[i][R] << ", G: " << (uint16_t)_palette_selected[i][G] << ", B: " << (uint16_t)_palette_selected[i][G] << std::endl;
    }

    for (uint8_t i = 0; i < 16; i++) {
        _ppu_bus_ptr->set_address(0x3F00 + i);
        uint8_t read = _ppu_bus_ptr->read_data();
        std::cout << (uint16_t)i << ": " << std::hex << (uint16_t)read << std::dec << std::endl;
    }
}

void pattern_table_preview::select_palette(void) {
    _palette_selected_index += 4; // move to next palette, which is usually 4 locations forward
    _palette_selected_index = _palette_selected_index % 16; // wrap this palette around if over 16. 
    update_palette_array(); // and update the palette
}