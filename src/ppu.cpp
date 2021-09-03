#include "ppu.h"

ppu::ppu(bus* cpu_bus_ptr, bus* ppu_bus_ptr, cpu* cpu_ptr) {
    _cpu_bus_ptr = cpu_bus_ptr;
    _ppu_bus_ptr = ppu_bus_ptr;
    _cpu_ptr = cpu_ptr;

    _colour_depth = 4; 
    _raw_pixel_data.resize(FRAME_ARRAY_SIZE * _colour_depth); 

    reset();
}

void ppu::bg_set_pixel(void) {
    // update our nametable x and y 
    _nametable_x = _clock_pulse_x / _sprite_width;
    _nametable_y = _scanline_y / SPRITE_HEIGHT;        

    // boil down the scanline and clock to the x and y within the tile,
    uint8_t pattern_table_row_x_index = _sprite_width - (_clock_pulse_x % _sprite_width) - 1;  // need to offset by sprite width as we read MSB as left most pixel
    uint8_t pattern_table_row_y_index = _scanline_y % SPRITE_HEIGHT;

    // extract the bit from the pattern table, in plane 0 and 1, 
    uint8_t plane_0_bit = (_pattern_row_plane_0_cache[_nametable_x][pattern_table_row_y_index] & (1 << pattern_table_row_x_index)) >> pattern_table_row_x_index;
    uint8_t plane_1_bit = (_pattern_row_plane_1_cache[_nametable_x][pattern_table_row_y_index] & (1 << pattern_table_row_x_index)) >> pattern_table_row_x_index;

    // get the pixel pattern and generate a colour index from it.
    uint8_t pattern_pixel = plane_0_bit | (plane_1_bit << 1);

    // calculate the x and y within the cached attribute table, basically split this into the 2x2 matrix inside the table
    uint8_t attribute_palette_x = (_nametable_x / 2) % 2;                                                       
    uint8_t attribute_palette_y = (_nametable_y / 2) % 2; 
    uint8_t attribute_palette_index = (attribute_palette_y * 2) + attribute_palette_x;          // should generate an index between 0 and 3. but in order 3-0 
    uint8_t attribute_bits = (_attribute_table_row_cache[_clock_pulse_x / 32] >> (attribute_palette_index * 2)) & 0x03;   // shift right to get the lowest 2 bits, clear the upper 6 bits
    _result_pixel = _background_palette_cache[((attribute_bits * 4) + pattern_pixel) % BACKGROUND_PALETTES];
}

void ppu::cache_bg_palettes(void) {
    // At the start of every frame, we'll cache the 16 palettes to avoid doing copious amounts of reads per frame
    for (uint8_t i = 0; i < BACKGROUND_PALETTES; i++) {
        _ppu_bus_ptr->set_address(PALETTE_RAM_INDEX_START + i);
        _background_palette_cache[i] = _ppu_bus_ptr->read_data();
    }
}

bool ppu::bg_rendering_enabled(void) {
    return check_bit(_PPU_mask_register, PPUMASK_SHOW_BACKGROUND) == 0 ? false : true;
}

bool ppu::bg_left_eight_pixels_enabled(void) {
    if (_clock_pulse_x >= 8) {
        return true;
    }
    return check_bit(_PPU_mask_register, PPUMASK_SHOWLEFT_BG) == 0 ? false : true;
}

void ppu::cache_nametable_row(void) {
    /*
        Cache entire row of the nametable
        Running this again and again is safe as it will first check the scanline to see if it's ready to cache a new row
    */

    if (_scanline_y % SPRITE_HEIGHT == 0 && _clock_pulse_x == 0) {    // Do this only once at the start of the scanline
        uint16_t base_nametable_address = NAMETABLE_0_START; // failsafe

        switch(_PPU_control_register & 0x03) {  // we only want to read the bottom 3 bits of this register
            case 0:
                base_nametable_address = NAMETABLE_0_START;
                break; 
            case 1:
                base_nametable_address = NAMETABLE_1_START;
                break; 
            case 2:
                base_nametable_address = NAMETABLE_2_START;
                break; 
            case 3:
                base_nametable_address = NAMETABLE_3_START;
                break; 
        }

        // boil down the scanline and clock to determined nametable x and y position
        _nametable_y = _scanline_y / SPRITE_HEIGHT;            
        uint16_t nametable_index = base_nametable_address + (_nametable_y * NAMETABLE_WIDTH);   // we don't need the x offset since we are at x=0

        for (uint8_t i = 0; i < NAMETABLE_WIDTH; i++) {  // todo - add 1 for allow room for scrolling
            _ppu_bus_ptr->set_address(nametable_index + i);  // todo - add in edge detection, and load in set of next tiles as needed
            _nametable_row_cache[i] = _ppu_bus_ptr->read_data();
        }
    }
}

void ppu::cache_pattern_row(void) {
    /*
        Cache entire row of the patterns from the nametable
        Running this again and again is safe as it will first check the scanline to see if it's ready to cache a new row
    */
    if (_scanline_y % SPRITE_HEIGHT == 0 && _clock_pulse_x == 0) {    // Do this only once at the start of the scanline
        // update our nametable x as needed
        _nametable_x = _clock_pulse_x / _sprite_width;

        uint16_t pattern_offset = 0; 

        if (check_bit(_PPU_control_register, PPUCTRL_BG_PATTERN_TABLE_ADDR)) {
            pattern_offset += 0x1000;
        }

        for (uint8_t i = 0; i < NAMETABLE_WIDTH; i++) {         // todo - add 1 for allow room for scrolling
            int used_before_index = -1;     // initially set to say this pattern is unique

            // first check to see if this address has been used before
            if (_nametable_x > 0) {
                for (uint8_t j = 0; j < NAMETABLE_WIDTH; j++) {
                    if (_nametable_row_cache[i] == _nametable_row_cache[j]) { // if the same address was found before
                        used_before_index = j;
                        break; 
                    }
                }
            }

            if (used_before_index == -1) { // check if the pattern is unique
                uint16_t _pattern_address = (_nametable_row_cache[i] << 4) + pattern_offset;        // convert to the actual tile address, e.g 0x0020 becomes 0x020     

                // load in each byte representing a row.
                for (uint8_t y = 0; y < SPRITE_HEIGHT; y++) {
                    _ppu_bus_ptr->set_address(_pattern_address + y);       
                    _pattern_row_plane_0_cache[i][y] = _ppu_bus_ptr->read_data();            // read bit plane 0
                    _ppu_bus_ptr->set_address(_pattern_address + y + 8);                    
                    _pattern_row_plane_1_cache[i][y] = _ppu_bus_ptr->read_data();            // then bit plane 1, 8 bits later
                }   
            }
            else {
                _pattern_row_plane_0_cache[i] = _pattern_row_plane_0_cache[used_before_index];            // std::array make this copy operation possible
                _pattern_row_plane_1_cache[i] = _pattern_row_plane_1_cache[used_before_index];           
            }
        }                  
    }
}

void ppu::cache_attribute_table_row(void) {
    /*
        Cache entire row of the attribute table at end nametable
        Running this again and again is safe as it will first check the scanline to see if it's ready to cache a new row
    */

    if (_scanline_y % SPRITE_HEIGHT == 0 && _clock_pulse_x == 0) {    // Do this only once at the start of the scanline
        uint16_t base_attribute_table_address;

        switch(_PPU_control_register & 0x03) {  // we only want to read the bottom 3 bits
            case 0:
                base_attribute_table_address = NT_0_ATTRIBUTE_TABLE;
                break; 
            case 1:
                base_attribute_table_address = NT_1_ATTRIBUTE_TABLE;
                break; 
            case 2:
                base_attribute_table_address = NT_2_ATTRIBUTE_TABLE;
                break; 
            case 3:
                base_attribute_table_address = NT_3_ATTRIBUTE_TABLE;
                break; 
        }

        // boil down the scanline and clock to determined attribute table x and y position
        //uint16_t attribute_table_x = _clock_pulse_x / 32;     // doesn't seem we need this, but good to have     
        uint16_t attribute_table_y = _scanline_y / 32;         

        uint16_t attribute_table_index = base_attribute_table_address + (attribute_table_y * ATTRTABLE_WIDTH);   // we don't need the x offset since we are at x=0

        for (uint8_t i = 0; i < ATTRTABLE_WIDTH; i++) {  // todo - add 1 for allow room for scrolling
            _ppu_bus_ptr->set_address(attribute_table_index + i);  // todo - add in edge detection, and load in set of next tiles as needed
            _attribute_table_row_cache[i] = _ppu_bus_ptr->read_data();
        }
    }
}

void ppu::cycle(void) {
    cache_nametable_row();
    cache_pattern_row();
    cache_attribute_table_row();

    // draw everything within the rendering area
    if (_clock_pulse_x <= FRAME_WIDTH && _scanline_y >= 0 && _scanline_y <= FRAME_HEIGHT) {
        bg_set_pixel();  

        // Check that background rendering is enabled and insert in to raw pixel data
        uint32_t pixel_index = (FRAME_WIDTH * 4 * _scanline_y) + (_clock_pulse_x * 4);        

        if (bg_rendering_enabled() && bg_left_eight_pixels_enabled() && pixel_index + 3 <= FRAME_ARRAY_SIZE * 4) {
            _raw_pixel_data[pixel_index + 0] = NTSC_PALETTE[_result_pixel][B];       
            _raw_pixel_data[pixel_index + 1] = NTSC_PALETTE[_result_pixel][G];       
            _raw_pixel_data[pixel_index + 2] = NTSC_PALETTE[_result_pixel][R];              
            _raw_pixel_data[pixel_index + 3] = SDL_ALPHA_OPAQUE;   
        }
    }

    _clock_pulse_x++;     // clock out pixel by pixel to output buffer

    // If rendering is enabled, the PPU increments the vertical position in v. The effective Y scroll coordinate is incremented
    /*if (_clock_pulse_x == 256) {
        // TODO!
    }*/

    // If rendering is enabled, the PPU copies all bits related to horizontal position from t to v:
    /*if (_clock_pulse_x == 257 && bg_rendering_enabled()) {
        _current_vram_address = _temp_vram_address;
    }*/

    if (_clock_pulse_x >= PIXELS_PER_SCANLINE) {
        _clock_pulse_x = 0;
        _scanline_y++;
        
        if (_scanline_y >= SCANLINES_PER_FRAME) {
            _scanline_y = -1;
            _frame_count++; // indicate a completed frame (at least the visible portion)
            _frame_complete_flag = true;
            cache_bg_palettes();    // cache new palettes at start of frame only
        }
    }

    // x:1 y:241 is when vertical blank is set
    if (_scanline_y == 241 && _clock_pulse_x == 1) {
        vertical_blank();
    }

    // x:1 y:261 is when vertical blank is cleared
    if (_scanline_y == 261 && _clock_pulse_x == 1) {
        // clear the VBlank bit, signifying that we are busy
        _PPU_status_register &= ~(1 << PPUSTATUS_VERTICAL_BLANK); // clear the vertical blank after the status reads
    }
}

void ppu::reset(void) {
    //reset statuses back to starting position
    _PPU_control_register = 0;
    _PPU_status_register = 0;
    _PPU_oam_data_status_register = 0;

    // set the palette to all black (0x3F, the final entry in the palette) If you do this before registering bus devices, this will do nothing
    for(uint8_t i = 0; i < PALETTE_RAM_SIZE; i++) {
        _ppu_bus_ptr->set_address(PALETTE_RAM_INDEX_START + i);
        _ppu_bus_ptr->write_data(0x3F);
    }

    _write_toggle = 0;
    _fine_x_scroll = 0;

    _current_vram_address = 0; // reset the video memory address

    // set the vertical blank bit to 1, indicating the PPU is busy
    _PPU_status_register |= (1 << PPUSTATUS_VERTICAL_BLANK);    

    _scanline_y = 0;
    _clock_pulse_x = 0;

    _sprite_width = 8; // we'll default to 8x wide for safety, but updating PPUCTRL will overwrite this

    _frame_count = 0;
    _frame_complete_flag = false;

    for (auto& i: _raw_pixel_data) i = 0;   // clear the pixel data

    // reset our ppu cached variables
	_nametable_x = 0; 
    _nametable_y = 0; 
    _result_pixel = 0;
}

void ppu::trigger_cpu_NMI(void) {
    _cpu_ptr->NMI();
}

uint8_t ppu::read(uint16_t address) {
    // the PPU ports are mirrored every 8 bytes
    address &= PPU_ADDRESS_SPACE_END;  
    uint8_t data = 0; 

    switch (address) { 
        // some ports are read only
        case PPUSTATUS:
            _write_toggle = 0;      // allow PPUADDRESS and PPUSCROLL to start their 2x write process
            data = (_PPU_status_register & 0xE0) | (_buffered_read & 0x1F); // this will only allow you to read the top 3 bits, the lower 5 bits are data that was read last time 
            _PPU_status_register &= ~(1 << PPUSTATUS_VERTICAL_BLANK); // clear the vertical blank after the status reads
            // TODO: Race Condition Warning: Reading PPUSTATUS within two cycles of the start of vertical blank will return 0 in bit 7 but clear the latch anyway, causing NMI to not occur that frame. See NMI and PPU_frame_timing for details.
            break;
        case OAMDATA:
            data = _PPU_oam_data_status_register;
            break;
        case PPUDATA:
            _ppu_bus_ptr->set_address(_current_vram_address); 
            data = _buffered_read;

            // ppu reads are behind by one cycle unless reading from palette memory  
            if (_current_vram_address >= PALETTE_RAM_INDEX_START && _current_vram_address <= PALETTE_RAM_MIRROR_END)
            {
                data = _ppu_bus_ptr->read_data();  // read directly
            }
            else {    
                data = _buffered_read;     // read out what was read last cycle
                _buffered_read = _ppu_bus_ptr->read_data();   // buffer the latest data
            }

            increment_video_memory_address();           
            break;
    }  
    return data;
}

void ppu::write(uint16_t address, uint8_t data) {
    // the PPU ports are mirrored every 8 bytes
    address &= PPU_ADDRESS_SPACE_END;  

    switch (address) { 
        // not all of these ports have write access  
        case PPUCTRL:
            _PPU_control_register = data & 0xFC;             // just the top 6 bits to be set here
            _temp_vram_address = (data & 0x03) << 10;    // the bottom two bits placed into t register, moved up t: ...GH.. ........ <- d: ......GH
            _sprite_width = (check_bit(_PPU_control_register, PPUCTRL_SPRITE_SIZE) == 0 ? 8 : 16); // update the sprite width
            break;
        case PPUMASK:
            _PPU_mask_register = data;
            break;            
        case OAMADDR:
            _PPU_oam_addr_status_register = data;
            break;            
        case OAMDATA:
            _PPU_oam_data_status_register = data;
            break;     
        case PPUSCROLL: 
            if (_write_toggle == 0) {
                uint8_t trimmed_scroll_data = data >> 3; // t: ....... ...ABCDE <- d: ABCDE...
                _temp_vram_address = trimmed_scroll_data;
                _fine_x_scroll = (data & 0x07); // just the bottom 3 bits.  x:              FGH <- d: .....FGH
                _write_toggle = 1;          
            }
            else if (_write_toggle == 1) {
                // this doesn't make any sense to me. It's just how it works aparently..
                uint16_t scroll_plane_0 = data << 12;                       // t: FGH.... ........ <- d: ABCDEFGH
                uint16_t scroll_plane_1 = (data >> 2) << 5;                 // t: .....AB CDE..... <- d: ABCDEFGH
                _temp_vram_address = scroll_plane_0 | scroll_plane_1;       // t: FGH..AB CDE..... <- d: ABCDEFGH
                _write_toggle = 0;
            }
            break;
        case PPUADDR:
            if (_write_toggle == 0) {
                // as per PPU Scrolling behaviour, certain bits need to be clipped off for this to function accurately. see this guide: https://wiki.nesdev.com/w/index.php/PPU_scrolling
                _temp_vram_address = data & 0x3F;  // clip the top two bits off
                _temp_vram_address &= 0x7FF; // clip the top bit off
                _write_toggle = 1;          
            } 
            else if (_write_toggle == 1) {
                // this is supposed to shift the new 8 bits of data into the temp register first, then copy from temp to current vram address, this was more convenient.
                _current_vram_address = _temp_vram_address << 8;   // shift the upper 8 bits into position
                _current_vram_address |= data;                     // read the lower 8 bits on the second read
                _write_toggle = 0;
            }
            break;              
        case PPUDATA:
            _ppu_bus_ptr->set_address(_current_vram_address); // for safety we may want to chop off the top two bits of the address
            _ppu_bus_ptr->write_data(data);     
            increment_video_memory_address();
            break;            
    }  
}

uint16_t ppu::get_video_memory_address(void) {
    return _current_vram_address;
}

bool ppu::get_vertical_blank(void) {
    return check_bit(_PPU_status_register, 7);
}

void ppu::vertical_blank(void) {    
    // set the vertical blank bit in the Status register, indicating to the rest of the system that we are okay to start writing pixel data
    _PPU_status_register |= (1 << PPUSTATUS_VERTICAL_BLANK);

    // trigger the NMI if that PPUCTRL register was set
    if (check_bit(_PPU_control_register, PPUCTRL_VERTICAL_BLANK_NMI) == 1) {
        trigger_cpu_NMI(); // temporarily disabling as this is causing an odd bug while the rest of the implementation isn't switched on. We'll come back to this when the rest of the code is fully implemented
    }
}

void ppu::increment_video_memory_address(void) {
    // PPUCTRL register is read, and bit 3 determines if we increment by 1 (incrementing x) or 32 (incrementing our y)
    _current_vram_address += (check_bit(_PPU_control_register, PPUCTRL_VRAM_INCREMENT) == 0 ? 1 : 32);
}

uint16_t ppu::get_clock_pulses(void) {
    return _clock_pulse_x;
}

uint16_t ppu::get_x(void) {     // different terminology, same as above
    return _clock_pulse_x;
}

int ppu::get_y(void) {
    return _scanline_y;
}

uint32_t ppu::get_frame_count(void) {
    return _frame_count;
}

bool ppu::get_frame_complete_flag(void) {
    return _frame_complete_flag; 
}

void ppu::clear_frame_complete_flag(void) {
    _frame_complete_flag = false;     // reset so that the renderer can set it
}