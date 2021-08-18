#include "ppu.h"

ppu::ppu(bus* cpu_bus_ptr, bus* ppu_bus_ptr, cpu* cpu_ptr) {
    _cpu_bus_ptr = cpu_bus_ptr;
    _ppu_bus_ptr = ppu_bus_ptr;
    _cpu_ptr = cpu_ptr;

    _colour_depth = 4; 
    _raw_pixel_data.resize(FRAME_ARRAY_SIZE * _colour_depth); 

    reset();

    _frame_count = 0;
}

void ppu::cycle(void) {
    // clock out pixel by pixel to output buffer
    clock_pulse_x++;
    
    // rendering area
    if (/*clock_pulse_x >= 0 &&*/ clock_pulse_x <= FRAME_WIDTH && scanline_y >= 0 && scanline_y <= FRAME_HEIGHT) {
        // boil down the scaline and clock to the tile on the nametable
        uint16_t nametable_index_x = clock_pulse_x / _sprite_width; // forcing into unsigned integer will round down
        uint16_t nametable_index_y = scanline_y / SPRITE_HEIGHT;            
        uint16_t nametable_index_offset = (nametable_index_y * NAMETABLE_WIDTH) + nametable_index_x;

        _ppu_bus_ptr->set_address(NAMETABLE_0_START + nametable_index_offset);
        uint8_t pattern_address = _ppu_bus_ptr->read_data();

        // boil down the scanline and clock to the x and y within the pattern table,
        uint8_t sprite_pixel_index_x = _sprite_width - (clock_pulse_x % _sprite_width);  // need to offset by sprite width as we read MSB
        // We don't need the y position as we will load the entire row

        _ppu_bus_ptr->set_address(pattern_address);
        // read the row data from the pattern table
        uint8_t row_data_plane_0 = _ppu_bus_ptr->read_data(); 

        _ppu_bus_ptr->set_address(pattern_address + 8);
        uint8_t row_data_plane_1 = _ppu_bus_ptr->read_data(); 

        // extract the bit from the pattern table, in plane 0 and 1, 
        uint8_t plane_0_bit = ((1 << sprite_pixel_index_x) & row_data_plane_0) >> sprite_pixel_index_x;
        uint8_t plane_1_bit = ((1 << sprite_pixel_index_x) & row_data_plane_1) >> sprite_pixel_index_x;

        // get the pixel pattern and generate a colour index from it.
        uint8_t pattern_pixel = (plane_0_bit << 1) & plane_1_bit;
        uint8_t palette_index = pattern_pixel; // TODO!

        // calculate an xy index from the scanline and clock
        uint32_t pixel_index = (FRAME_WIDTH * 4 * scanline_y) + (clock_pulse_x * 4);        

        /*uint8_t static_snow = rand() % 255;
        _raw_pixel_data[pixel_index + 0] = static_snow;
        _raw_pixel_data[pixel_index + 1] = static_snow;
        _raw_pixel_data[pixel_index + 2] = static_snow;                      
        _raw_pixel_data[pixel_index + 3] = SDL_ALPHA_OPAQUE;   */ 

        // draw a pixel from pattern table (background) if rendering is enabled
        if (check_bit(_PPU_mask_register, PPUMASK_SHOW_BACKGROUND) && pixel_index + 3 <= FRAME_ARRAY_SIZE) {   
            // look up the colour index from the NTSC palette and add to the rax pixel data
            _raw_pixel_data[pixel_index + 0] = NTSC_PALETTE[palette_index][R];
            _raw_pixel_data[pixel_index + 1] = NTSC_PALETTE[palette_index][G];
            _raw_pixel_data[pixel_index + 2] = NTSC_PALETTE[palette_index][B];                      
            _raw_pixel_data[pixel_index + 3] = SDL_ALPHA_OPAQUE;            
        }

        // draw a pixel from the OAM table (sprites) if rendering is enabled
        if (check_bit(_PPU_mask_register, PPUMASK_SHOW_SPRITES) && pixel_index + 3 <= FRAME_ARRAY_SIZE) {
            // todo
        }          
    }

    if (clock_pulse_x >= PIXELS_PER_SCANLINE) {
        clock_pulse_x = 0;
        scanline_y++;
        
        if (scanline_y >= SCANLINES_PER_FRAME) {
            scanline_y = -1;
            _frame_count++; // indicate a completed frame (at least the visible portion)
        }
    }

    // x:1 y:241 is when vertical blank is set
    if (scanline_y == 241 && clock_pulse_x == 1) {
        vertical_blank();
    }

    // x:1 y:261 is when vertical blank is cleared
    if (scanline_y == 261 && clock_pulse_x == 1) {
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

    scanline_y = 0;
    clock_pulse_x = 0;

    _sprite_width = 8; // we'll default to 8x wide for safety, but updating PPUCTRL will overwrite this
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

uint16_t ppu::get_x(void) {
    return clock_pulse_x;
}

uint16_t ppu::get_y(void) {
    return scanline_y;
}

unsigned long ppu::get_frame_count(void) {
    return _frame_count;
}