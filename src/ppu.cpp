#include "ppu.h"

ppu::ppu(bus* cpu_bus_ptr, bus* ppu_bus_ptr, cpu* cpu_ptr) {
    _cpu_bus_ptr = cpu_bus_ptr;
    _ppu_bus_ptr = ppu_bus_ptr;
    _cpu_ptr = cpu_ptr;

    _colour_depth = 4; 
    _raw_pixel_data.resize(FRAME_ARRAY_SIZE * _colour_depth); 

    reset();
}

void ppu::cycle(void) {
    bool new_pattern_address = false; 

    _clock_pulse_x++;     // clock out pixel by pixel to output buffer

    // manage what to do in the rendering area
    if (_clock_pulse_x <= FRAME_WIDTH && _scanline_y >= 0 && _scanline_y <= FRAME_HEIGHT) {
        // boil down the scaline and clock to the tile on the nametable
        // for now, just read from nametable 0
        uint16_t nametable_index_x = _clock_pulse_x / _sprite_width; // forcing into unsigned integer will round down
        uint16_t nametable_index_y = _scanline_y / SPRITE_HEIGHT;            
        uint16_t new_nametable_index_offset = 0x2000 + ((nametable_index_y * NAMETABLE_WIDTH) + nametable_index_x);

        if (new_nametable_index_offset != _nametable_index_offset) {
            _nametable_index_offset = new_nametable_index_offset; // buffer this

            // grab the tile ID from the nametable
            _ppu_bus_ptr->set_address(_nametable_index_offset);
            _pattern_address = _ppu_bus_ptr->read_data() << 4; // convert to the actual tile address, e.g 0x0020 becomes 0x020
            new_pattern_address = true; 
        }

        // boil down the scanline and clock to the x and y within the tile,
        uint8_t pattern_table_row_x_index = _sprite_width - (_clock_pulse_x % _sprite_width) - 1;  // need to offset by sprite width as we read MSB

        if (new_pattern_address) {
            uint8_t pattern_table_row_y_index = _scanline_y % SPRITE_HEIGHT;

            // read the row data from the pattern table
            _ppu_bus_ptr->set_address(_pattern_address + pattern_table_row_y_index);
            row_data_plane_0 = _ppu_bus_ptr->read_data(); 
            _ppu_bus_ptr->set_address(_pattern_address + pattern_table_row_y_index + 8);
            row_data_plane_1 = _ppu_bus_ptr->read_data(); 
        }

        // extract the bit from the pattern table, in plane 0 and 1, 
        uint8_t plane_0_bit = (row_data_plane_0 & (1 << pattern_table_row_x_index)) >> pattern_table_row_x_index;
        uint8_t plane_1_bit = (row_data_plane_1 & (1 << pattern_table_row_x_index)) >> pattern_table_row_x_index;

        // get the pixel pattern and generate a colour index from it.
        uint8_t pattern_pixel = plane_0_bit | (plane_1_bit << 1);

        // pull down the pallette from the OAM and determine which palette to apply
        //0x3F00 refers to the transparent color
        //3F01, 3F05, 3F09, 3F0D are used for background palettes

        // for now, just read from Nametable 0 attribute data
        uint16_t attribute_table_index_x = _clock_pulse_x / 32; 
        uint16_t attribute_table_index_y = _scanline_y / 32;   
        uint16_t new_attribute_table_index = 0x23C0 + ((attribute_table_index_y * 8) + attribute_table_index_x);

        if (new_attribute_table_index != _attribute_table_index) {
            _attribute_table_index = new_attribute_table_index;

            _ppu_bus_ptr->set_address(_attribute_table_index);
            uint8_t new_attribute_table_data = _ppu_bus_ptr->read_data();

            if (new_attribute_table_data != _attribute_table_data) {
                _attribute_table_data = new_attribute_table_data;
            }
        }

        uint8_t attribute_palette_x = nametable_index_x % 2; 
        uint8_t attribute_palette_y = nametable_index_y % 2; 
        uint8_t attribute_palette_index = (attribute_palette_y * 2) + attribute_palette_x; // should generate a number between 0 and 3. 
        uint8_t attribute_bits = (_attribute_table_data >> (attribute_palette_index * 2)) & 0x03; // shift right to get the lowest 2 bits, clear the upper 6 bits

        _ppu_bus_ptr->set_address(0x3F00 + (attribute_bits * 4) + pattern_pixel);
        uint8_t p = _ppu_bus_ptr->read_data();

        // calculate an xy index from the scanline and clock
        uint32_t pixel_index = (FRAME_WIDTH * 4 * _scanline_y) + (_clock_pulse_x * 4);        

        // draw a pixel from pattern table (background) if rendering is enabled
        if (check_bit(_PPU_mask_register, PPUMASK_SHOW_BACKGROUND) && pixel_index + 3 <= FRAME_ARRAY_SIZE * 4) {   
            // look up the colour index from the NTSC palette and add to the rax pixel data
            _raw_pixel_data[pixel_index + 0] = NTSC_PALETTE[p][B];       
            _raw_pixel_data[pixel_index + 1] = NTSC_PALETTE[p][G];       
            _raw_pixel_data[pixel_index + 2] = NTSC_PALETTE[p][R];              
            _raw_pixel_data[pixel_index + 3] = SDL_ALPHA_OPAQUE;             
        }

        // draw a pixel from the OAM table (sprites) if rendering is enabled
        if (check_bit(_PPU_mask_register, PPUMASK_SHOW_SPRITES) && pixel_index + 3 <= FRAME_ARRAY_SIZE * 4) {
            // todo
        } 
    }

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

    _nametable_index_offset = 0;
    _pattern_address = 0;    
    _attribute_table_index = 0;

    for (auto& p : _palette_info) p = 0; 
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
            //_ppu_bus_ptr->set_address(_video_memory_address); // may need this later? seems to cause an issue with debug displays
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