#include "ppu.h"

ppu::ppu(bus* cpu_bus_ptr, bus* ppu_bus_ptr, cpu* cpu_ptr) {
    _cpu_bus_ptr = cpu_bus_ptr;
    _ppu_bus_ptr = ppu_bus_ptr;
    _cpu_ptr = cpu_ptr;

    _colour_depth = 4; 
    _raw_pixel_data.resize(FRAME_ARRAY_SIZE * _colour_depth); 

    reset();
}

void ppu::bg_read_pattern_pixel_from_cache(void) {
    // update our nametable x and y 
    _nametable_x = _clock_pulse_x / _sprite_width;
    _nametable_y = _scanline_y / SPRITE_HEIGHT;        

    // boil down the scanline and clock to the x and y within the tile,
    uint8_t pattern_table_row_x_index = _sprite_width - (_clock_pulse_x % _sprite_width) - 1;  // need to offset by sprite width as we read MSB as left most pixel
    uint8_t pattern_table_row_y_index = _scanline_y % SPRITE_HEIGHT;

    // extract the bit from the pattern table, in plane 0 and 1, 
    uint8_t plane_0_bit = (pattern_row_plane_0_cache[_nametable_x][pattern_table_row_y_index] & (1 << pattern_table_row_x_index)) >> pattern_table_row_x_index;
    uint8_t plane_1_bit = (pattern_row_plane_1_cache[_nametable_x][pattern_table_row_y_index] & (1 << pattern_table_row_x_index)) >> pattern_table_row_x_index;

    // get the pixel pattern and generate a colour index from it.
    _pattern_pixel = plane_0_bit | (plane_1_bit << 1);
}

void ppu::bg_read_attribute_table(void) {
    // pull down the pallette from the OAM and determine which palette to apply
    //0x3F00 refers to the transparent color, 3F01, 3F05, 3F09, 3F0D are used for background palettes
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

    _attribute_table_x = _clock_pulse_x / 32;          
    _attribute_table_y = _scanline_y / 32;   
    uint16_t new_attribute_table_index = base_attribute_table_address + ((_attribute_table_y * 8) + _attribute_table_x);    // attribute tables are 8x8

    if (new_attribute_table_index != _attribute_table_index) {
        _attribute_table_index = new_attribute_table_index;

        _ppu_bus_ptr->set_address(_attribute_table_index);
        uint8_t new_attribute_table_data = _ppu_bus_ptr->read_data();

        if (new_attribute_table_data != _attribute_table_data) {
            _attribute_table_data = new_attribute_table_data;
        }
    }

    uint8_t attribute_palette_x = (_nametable_x / 2) % 2;                                                       
    uint8_t attribute_palette_y = (_nametable_y / 2) % 2; 
    uint8_t attribute_palette_index = (attribute_palette_y * 2) + attribute_palette_x;          // should generate an index between 0 and 3. but in order 3-0 
    uint8_t attribute_bits = (_attribute_table_data >> (attribute_palette_index * 2)) & 0x03;   // shift right to get the lowest 2 bits, clear the upper 6 bits
    uint16_t new_palette_address = PALETTE_RAM_INDEX_START + (attribute_bits * 4) + _pattern_pixel; 

    if (new_palette_address != _palette_address) {
        _palette_address = new_palette_address;
        _ppu_bus_ptr->set_address(new_palette_address);
        _result_pixel = _ppu_bus_ptr->read_data();
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
            nametable_row_cache[i] = _ppu_bus_ptr->read_data();
        }
    }
}

void ppu::cache_pattern_row(void) {
    /*
        Cache entire row of the patterns from the nametable
        Running this again and again is safe as it will first check the scanline to see if it's ready to cache a new row
    */
    if (_scanline_y % SPRITE_HEIGHT == 0 && _clock_pulse_x == 0) {    // Do this only once at the start of the scanline
        uint16_t pattern_offset = 0; 

        if (check_bit(_PPU_control_register, PPUCTRL_BG_PATTERN_TABLE_ADDR)) {
            pattern_offset += 0x1000;
        }

        for (uint8_t i = 0; i < NAMETABLE_WIDTH; i++) {         // todo - add 1 for allow room for scrolling
            uint16_t _pattern_address = (nametable_row_cache[i] << 4) + pattern_offset;        // convert to the actual tile address, e.g 0x0020 becomes 0x020     

            // todo, do a quick check to see if this has been seen before

            // load in each byte representing a row.
            for (uint8_t y = 0; y < SPRITE_HEIGHT; y++) {
                _ppu_bus_ptr->set_address(_pattern_address + y);       
                pattern_row_plane_0_cache[i][y] = _ppu_bus_ptr->read_data();            // read bit plane 0
                _ppu_bus_ptr->set_address(_pattern_address + y + 8);                    
                pattern_row_plane_1_cache[i][y] = _ppu_bus_ptr->read_data();            // then bit plane 1, 8 bits later
            }
        }                  
    }
}

void ppu::cache_attribute_table_row(void) {

}

void ppu::cycle(void) {
    cache_nametable_row();
    cache_pattern_row();
    cache_attribute_table_row();

    // draw everything within the rendering area
    if (_clock_pulse_x <= FRAME_WIDTH && _scanline_y >= 0 && _scanline_y <= FRAME_HEIGHT) {
        //bg_read_nametable();           // Read data from the nametable, maintaining the nt x, y and offset
        //bg_read_pattern_table();       // Read from the pattern table
        bg_read_pattern_pixel_from_cache();
        bg_read_attribute_table();     // Read from the palette information from attribute table

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

    if (_clock_pulse_x >= PIXELS_PER_SCANLINE) {
        _clock_pulse_x = 0;
        _scanline_y++;
        
        if (_scanline_y >= SCANLINES_PER_FRAME) {
            _scanline_y = -1;
            _frame_count++; // indicate a completed frame (at least the visible portion)
            _frame_complete_flag = true;
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

    _address_latch = false; 
    _addr_second_write = false; 
    _scroll_second_write = false; 

    _video_memory_address = 0; // reset the video memory address

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
    _attribute_table_x = 0;
    _attribute_table_y = 0;
	_nametable_index = 0; 
    _pattern_address = 0; 
    _attribute_table_index = 0; 
    _palette_address = 0;
	_row_data_plane_0 = 0; 
    _row_data_plane_1 = 0; 
    _attribute_table_data = 0;
    _pattern_pixel = 0; 
    _result_pixel = 0;
	_read_new_pattern = false;

    /*for (auto& i: nametable_row_cache) i = 0;       // clear the nametable row cache
    for (auto& i: pattern_row_cache)
        for () // clear the nametable row cache*/
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
            _address_latch = true;  // allow PPUADDRESS and PPUSCROLL to start their 2x write process
            data = _PPU_status_register; 
            _PPU_status_register &= ~(1 << PPUSTATUS_VERTICAL_BLANK); // clear the vertical blank after the status reads
            // TODO: Race Condition Warning: Reading PPUSTATUS within two cycles of the start of vertical blank will return 0 in bit 7 but clear the latch anyway, causing NMI to not occur that frame. See NMI and PPU_frame_timing for details.
            break;
        case OAMDATA:
            data = _PPU_oam_data_status_register;
            break;
        case PPUDATA:
            _ppu_bus_ptr->set_address(_video_memory_address); 
            data = _ppu_bus_ptr->read_data();       // we rely on setting the address via PPUADDR writes, it's possible to get junk data from this if you just go straight for the read
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
            _PPU_control_register = data;
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
        case PPUADDR:
            // this requires two writes to work and only works if the address latch is on
            if (_address_latch) {
                if (_addr_second_write) {
                    _video_memory_address = _ppu_addr_temp_register << 8;   // shift the upper 8 bits into position
                    _video_memory_address |= data;                     // read the lower 8 bits on the second read
                    _addr_second_write = false;         // reset back to default
                }
                else {
                    _ppu_addr_temp_register = data;  // this reads the upper 8 bits
                    _addr_second_write = true;       // enable the second write
                } 
            }
            break;              
        case PPUDATA:
            _ppu_bus_ptr->set_address(_video_memory_address); 
            _ppu_bus_ptr->write_data(data);     
            increment_video_memory_address();
            break;            
    }  
}

uint16_t ppu::get_video_memory_address(void) {
    return _video_memory_address;
}

bool ppu::get_address_latch(void) {
    return _address_latch;
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
    // PPUCTRL register is read, and bit 3 determines if we increment by 1 (going x) or 32 (incrementing our y)
    _video_memory_address += (check_bit(_PPU_control_register, PPUCTRL_VRAM_INCREMENT) == 0 ? 1 : 32);
    _ppu_bus_ptr->set_address(_video_memory_address); 
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