#include "ppu.h"

ppu::ppu(bus* cpu_bus_ptr, bus* ppu_bus_ptr, cpu* cpu_ptr) {
    _cpu_bus_ptr = cpu_bus_ptr;
    _ppu_bus_ptr = ppu_bus_ptr;
    _cpu_ptr = cpu_ptr;

    _colour_depth = 4; 
    _raw_pixel_data.resize(FRAME_ARRAY_SIZE * _colour_depth); 

    reset();
}

/*void ppu::bg_set_pixel(void) {
    // update our nametable x and y 
    uint16_t cache_x = _clock_pulse_x / SPRITE_WIDTH;
    uint16_t cache_y = _scanline_y / _sprite_height;        

    // boil down the scanline and clock to the x and y within the tile,
    uint8_t pattern_table_row_x_index = SPRITE_WIDTH - (_clock_pulse_x % SPRITE_WIDTH) - 1;  // need to offset by sprite width as we read MSB as left most pixel
    uint8_t pattern_table_row_y_index = _scanline_y % _sprite_height;

    // extract the bit from the pattern table, in plane 0 and 1, 
    uint8_t plane_0_bit = (_pattern_row_plane_0_cache[cache_x][pattern_table_row_y_index] & (1 << pattern_table_row_x_index)) >> pattern_table_row_x_index;
    uint8_t plane_1_bit = (_pattern_row_plane_1_cache[cache_x][pattern_table_row_y_index] & (1 << pattern_table_row_x_index)) >> pattern_table_row_x_index;

    // get the pixel pattern and generate a colour index from it.
    uint8_t pattern_pixel = plane_0_bit | (plane_1_bit << 1);

    // calculate the x and y within the cached attribute table, basically split this into the 2x2 matrix inside the table
    uint8_t attribute_palette_x = (cache_x / 2) % 2;                                                       
    uint8_t attribute_palette_y = (cache_y / 2) % 2; 
    uint8_t attribute_palette_index = (attribute_palette_y * 2) + attribute_palette_x;          // should generate an index between 0 and 3. but in order 3-0 
    uint8_t attribute_bits = (_attribute_table_row_cache[_clock_pulse_x / 32] >> (attribute_palette_index * 2)) & 0x03;   // shift right to get the lowest 2 bits, clear the upper 6 bits
    _result_pixel = _background_palette_cache[((attribute_bits * 4) + pattern_pixel) % BACKGROUND_PALETTES];
}*/

/*void ppu::cache_bg_palettes(void) {
    // At the start of every frame, we'll cache the 16 palettes to avoid doing copious amounts of reads per frame
    for (uint8_t i = 0; i < BACKGROUND_PALETTES; i++) {
        _ppu_bus_ptr->set_address(PALETTE_RAM_INDEX_START + i);
        _background_palette_cache[i] = _ppu_bus_ptr->read_data();
    }
}*/

bool ppu::bg_rendering_enabled(void) {
    return check_bit(_PPU_mask_register, PPUMASK_SHOW_BACKGROUND) == 0 ? false : true;
}

bool ppu::fg_rendering_enabled(void) {
    return check_bit(_PPU_mask_register, PPUMASK_SHOW_SPRITES) == 0 ? false : true;
}

bool ppu::bg_left_eight_pixels_enabled(void) {
    if (_clock_pulse_x >= 8) {
        return true;
    }
    return check_bit(_PPU_mask_register, PPUMASK_SHOWLEFT_BG) == 0 ? false : true;
}

/*void ppu::cache_nametable_row(void) {
    //
    //    Cache entire row of the nametable
    //    Running this again and again is safe as it will first check the scanline to see if it's ready to cache a new row
    //

    if (_scanline_y % _sprite_height == 0 && _clock_pulse_x == 0) {    // Do this only once at the start of the scanline
        uint16_t base_nametable_address = NAMETABLE_0_START; // failsafe
        uint8_t nametable_select = (_current_vram_address.nametable_y << 1) | _current_vram_address.nametable_x; // should deliver a base name table index from 0-3

        switch(nametable_select) {  // we only want to read the bottom 3 bits of this register
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
        _nametable_y = _scanline_y / _sprite_height;            
        uint16_t nametable_index = base_nametable_address + (_nametable_y * NAMETABLE_WIDTH) + _current_vram_address.coarse_x;  

        for (uint8_t i = 0; i < NAMETABLE_WIDTH; i++) {  // todo - add 1 for allow room for scrolling
            _ppu_bus_ptr->set_address(nametable_index + i);  // todo - add in edge detection, and load in set of next tiles as needed
            _nametable_row_cache[i] = _ppu_bus_ptr->read_data();
        }
    }
}*/

/*void ppu::cache_pattern_row(void) {
    //
    //    Cache entire row of the patterns from the nametable
    //    Running this again and again is safe as it will first check the scanline to see if it's ready to cache a new row
    //
    if (_scanline_y % _sprite_height == 0 && _clock_pulse_x == 0) {    // Do this only once at the start of the scanline
        // update our nametable x as needed
        //_nametable_x = _clock_pulse_x / _sprite_width;
        _nametable_x = _current_vram_address.coarse_x;

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
                for (uint8_t y = 0; y < _sprite_height; y++) {
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
}*/

/*void ppu::cache_attribute_table_row(void) {
    //
    //    Cache entire row of the attribute table at end nametable
    //    Running this again and again is safe as it will first check the scanline to see if it's ready to cache a new row
    //

    if (_scanline_y % _sprite_height == 0 && _clock_pulse_x == 0) {    // Do this only once at the start of the scanline
        uint16_t base_attribute_table_address;
        uint8_t nametable_select = (_current_vram_address.nametable_y << 1) | _current_vram_address.nametable_x; // should deliver a base name table index from 0-3

        switch(nametable_select) {  // we only want to read the bottom 3 bits of this register
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
        //uint16_t attribute_table_y = _current_vram_address.coarse_y;

        uint16_t attribute_table_index = base_attribute_table_address + (attribute_table_y * ATTRTABLE_WIDTH);   // we don't need the x offset since we are at x=0

        for (uint8_t i = 0; i < ATTRTABLE_WIDTH; i++) {  // todo - add 1 for allow room for scrolling
            _ppu_bus_ptr->set_address(attribute_table_index + i);  // todo - add in edge detection, and load in set of next tiles as needed
            _attribute_table_row_cache[i] = _ppu_bus_ptr->read_data();
        }
    }
}*/

void ppu::increment_scroll_x(void) {
    if (bg_rendering_enabled() || fg_rendering_enabled()) {
        if (_current_vram_address.coarse_x == NAMETABLE_WIDTH - 1) {
            _current_vram_address.coarse_x = 0; // wrap around the name table address				
            _current_vram_address.nametable_x = ~_current_vram_address.nametable_x; // And flip the target nametable bits, being a 2x2 grid, we pretty much wrap from 0 -> 1 -> 0
        }
        else {
            _current_vram_address.coarse_x++;
        }
    }
}

void ppu::increment_scroll_y(void) {
    if (bg_rendering_enabled() || fg_rendering_enabled()) {
        if (_current_vram_address.fine_y < _sprite_height - 1) {
            _current_vram_address.fine_y++;
        }   
        else {
            // we have gone beyond SPRITE_HEIGHT
            _current_vram_address.fine_y = 0;

            // do we need to swap the vertical name table addresses?
                if (_current_vram_address.coarse_y == NAMETABLE_HEIGHT - 1) {   // are we on the last row? 
                    _current_vram_address.coarse_y = 0;
                    _current_vram_address.nametable_y = ~_current_vram_address.nametable_y; // And flip the target nametable bits, being a 2x2 grid, we pretty much wrap from 0 -> 1 -> 0 
                }
                else if (_current_vram_address.coarse_y == NAMETABLE_HEIGHT + 1) {// check to make sure we haven't gone beyond the nametable region and entered into attribute memory territory
                    _current_vram_address.coarse_y = 0;
                }
                else {  
                    _current_vram_address.coarse_y++; // only if safe to do so
                }
        }
    }
}

void ppu::bg_load_shifters(void) {
    // Prime the "in-effect" background tile shifters ready for outputting next
    // 8 pixels in scanline.
    if (bg_rendering_enabled()) {
        // Each PPU update we calculate one pixel. These shifters shift 1 bit along
        // feeding the pixel compositor with the binary information it needs. Its
        // 16 bits wide, because the top 8 bits are the current 8 pixels being drawn
        // and the bottom 8 bits are the next 8 pixels to be drawn. Naturally this means
        // the required bit is always the MSB of the shifter. However, "fine x" scrolling
        // plays a part in this too, whcih is seen later, so in fact we can choose
        // any one of the top 8 bits.
        _bg_shifter_pattern_lo = (_bg_shifter_pattern_lo & 0xFF00) | _bg_next_tile_lsb;
        _bg_shifter_pattern_hi = (_bg_shifter_pattern_hi & 0xFF00) | _bg_next_tile_msb;
        
        // Attribute bits do not change per pixel, rather they change every 8 pixels
        // but are synchronised with the pattern shifters for convenience, so here
        // we take the bottom 2 bits of the attribute word which represent which
        // palette is being used for the current 8 pixels and the next 8 pixels, and
        // "inflate" them to 8 bit words.
        _bg_shifter_attrib_lo  = (_bg_shifter_attrib_lo & 0xFF00) | ((_bg_next_tile_attrib & 0b01) ? 0xFF : 0x00);
        _bg_shifter_attrib_hi  = (_bg_shifter_attrib_hi & 0xFF00) | ((_bg_next_tile_attrib & 0b10) ? 0xFF : 0x00);
    }
}

void ppu::bg_update_shifters(void) {
    // Every cycle the shifters storing pattern and attribute information shift
    // their contents by 1 bit. This is because every cycle, the output progresses
    // by 1 pixel. This means relatively, the state of the shifter is in sync
    // with the pixels being drawn for that 8 pixel section of the scanline.    
    if (bg_rendering_enabled()) {
        _bg_shifter_pattern_lo <<= 1;          // Shifting background tile pattern row
        _bg_shifter_pattern_hi <<= 1;
        _bg_shifter_attrib_lo <<= 1;                     // Shifting palette attributes by 1
        _bg_shifter_attrib_hi <<= 1;
    }
}

void ppu::cycle(void) {   
    // A massive thank you to JavidX / OLC for working this out, we've used his code as the base for ours, with modification of course!
    // Most of the comments explaining this code is his work. 

    if (_scanline_y >= -1 && _scanline_y < 240) {      
        if (_scanline_y == 0 && _clock_pulse_x == 0) {
            _clock_pulse_x = 1; // Skip this cycle on the odd frame
        }

        if (_scanline_y == -1 && _clock_pulse_x == 1) {
            _PPU_status_register &= ~(1 << PPUSTATUS_VERTICAL_BLANK); // clear the vertical blank after the status reads
        }

        if ((_clock_pulse_x >= 2 && _clock_pulse_x < 258) || (_clock_pulse_x >= 321 && _clock_pulse_x < 338)) {
            bg_update_shifters();
           
            // In these cycles we are collecting and working with visible data
            // The "shifters" have been preloaded by the end of the previous
            // scanline with the data for the start of this scanline. Once we
            // leave the visible region, we go dormant until the shifters are
            // preloaded for the next scanline.

            uint16_t pattern_memory_selector; 
            uint16_t bg_next_tile_id_multiplied; 
            uint8_t offset;

            // Fortunately, for background rendering, we go through a fairly
            // repeatable sequence of events, every 2 clock cycles.
            switch ((_clock_pulse_x - 1) % 8) {
                case 0:
                    bg_load_shifters(); // load the bg tile into the shifter

                    // Fetch the next background tile ID
                    _ppu_bus_ptr->set_address(0x2000 | (_current_vram_address.reg & 0x0FFF));       // cut down to 12 bits that are relevant, offset by the selected nametable id
                    _bg_next_tile_id = _ppu_bus_ptr->read_data();

                    // Explanation:
                    // The bottom 12 bits of the loopy register provide an index into
                    // the 4 nametables, regardless of nametable mirroring configuration.
                    // nametable_y(1) nametable_x(1) coarse_y(5) coarse_x(5)
                    //
                    // Consider a single nametable is a 32x32 array, and we have four of them
                    //   0                1
                    // 0 +----------------+----------------+
                    //   |                |                |
                    //   |                |                |
                    //   |    (32x32)     |    (32x32)     |
                    //   |                |                |
                    //   |                |                |
                    // 1 +----------------+----------------+
                    //   |                |                |
                    //   |                |                |
                    //   |    (32x32)     |    (32x32)     |
                    //   |                |                |
                    //   |                |                |
                    //   +----------------+----------------+
                    //
                    // This means there are 4096 potential locations in this array, which
                    // just so happens to be 2^12!
                    break;
                case 2:
                    // Recall that each nametable has two rows of cells that are not tile
                    // information, instead they represent the attribute information that
                    // indicates which palettes are applied to which area on the screen.
                    // Importantly (and frustratingly) there is not a 1 to 1 correspondance
                    // between background tile and palette. Two rows of tile data holds
                    // 64 attributes. Therfore we can assume that the attributes affect
                    // 8x8 zones on the screen for that nametable. Given a working resolution
                    // of 256x240, we can further assume that each zone is 32x32 pixels
                    // in screen space, or 4x4 tiles. Four system palettes are allocated
                    // to background rendering, so a palette can be specified using just
                    // 2 bits. The attribute byte therefore can specify 4 distinct palettes.
                    // Therefore we can even further assume that a single palette is
                    // applied to a 2x2 tile combination of the 4x4 tile zone. The very fact
                    // that background tiles "share" a palette locally is the reason why
                    // in some games you see distortion in the colours at screen edges.

                    // As before when choosing the tile ID, we can use the bottom 12 bits of
                    // the loopy register, but we need to make the implementation "coarser"
                    // because instead of a specific tile, we want the attribute byte for a
                    // group of 4x4 tiles, or in other words, we divide our 32x32 address
                    // by 4 to give us an equivalent 8x8 address, and we offset this address
                    // into the attribute section of the target nametable.

                    // Reconstruct the 12 bit loopy address into an offset into the
                    // attribute memory

                    // "(_current_vram_address.coarse_x >> 2)"        : integer divide coarse x by 4,
                    //                                      from 5 bits to 3 bits
                    // "((_current_vram_address.coarse_y >> 2) << 3)" : integer divide coarse y by 4,
                    //                                      from 5 bits to 3 bits,
                    //                                      shift to make room for coarse x

                    // Result so far: YX00 00yy yxxx

                    // All attribute memory begins at 0x03C0 within a nametable, so OR with
                    // result to select target nametable, and attribute byte offset. Finally
                    // OR with 0x2000 to offset into nametable address space on PPU bus.    
                    _ppu_bus_ptr->set_address(0x23C0    | (_current_vram_address.nametable_y << 11)
                                                        | (_current_vram_address.nametable_x << 10)
                                                        | ((_current_vram_address.coarse_y >> 2) << 3)
                                                        | (_current_vram_address.coarse_x >> 2));

                    _bg_next_tile_attrib = _ppu_bus_ptr->read_data();
                    // Right we've read the correct attribute byte for a specified address,
                    // but the byte itself is broken down further into the 2x2 tile groups
                    // in the 4x4 attribute zone.

                    // The attribute byte is assembled thus: BR(76) BL(54) TR(32) TL(10)
                    //
                    // +----+----+              +----+----+
                    // | TL | TR |              | ID | ID |
                    // +----+----+ where TL =   +----+----+
                    // | BL | BR |              | ID | ID |
                    // +----+----+              +----+----+
                    //
                    // Since we know we can access a tile directly from the 12 bit address, we
                    // can analyse the bottom bits of the coarse coordinates to provide us with
                    // the correct offset into the 8-bit word, to yield the 2 bits we are
                    // actually interested in which specifies the palette for the 2x2 group of
                    // tiles. We know if "coarse y % 4" < 2 we are in the top half else bottom half.
                    // Likewise if "coarse x % 4" < 2 we are in the left half else right half.
                    // Ultimately we want the bottom two bits of our attribute word to be the
                    // palette selected. So shift as required...                
                    if (_current_vram_address.coarse_y & 0x02) {
                        _bg_next_tile_attrib >>= 4;
                    }
                    if (_current_vram_address.coarse_x & 0x02) {
                        _bg_next_tile_attrib >>= 2;
                    }
                    _bg_next_tile_attrib &= 0x03;       // cut off the top 6 bits
                    break;
                case 4:
                    // Fetch the next background tile LSB bit plane from the pattern memory
                    // The Tile ID has been read from the nametable. We will use this id to
                    // index into the pattern memory to find the correct sprite (assuming
                    // the sprites lie on 8x8 pixel boundaries in that memory, which they do
                    // even though 8x16 sprites exist, as background tiles are always 8x8).
                    //
                    // Since the sprites are effectively 1 bit deep, but 8 pixels wide, we
                    // can represent a whole sprite row as a single byte, so offsetting
                    // into the pattern memory is easy. In total there is 8KB so we need a
                    // 13 bit address.

                    // "(control.pattern_background << 12)"  : the pattern memory selector
                    //                                         from control register, either 0K
                    //                                         or 4K offset
                    // "((uint16_t)bg_next_tile_id << 4)"    : the tile id multiplied by 16, as
                    //                                         2 lots of 8 rows of 8 bit pixels
                    // "(_current_vram_address.fine_y)"                  : Offset into which row based on
                    //                                         vertical scroll offset
                    // "+ 0"                                 : Mental clarity for plane offset
                    // Note: No PPU address bus offset required as it starts at 0x0000
                    pattern_memory_selector = check_bit(_PPU_control_register, PPUCTRL_BG_PATTERN_TABLE_ADDR) << 12;
                    bg_next_tile_id_multiplied = _bg_next_tile_id << 4;
                    offset = 0; // plus zero for no offset

                    _ppu_bus_ptr->set_address(pattern_memory_selector + bg_next_tile_id_multiplied + _current_vram_address.fine_y + offset);
                    _bg_next_tile_lsb = _ppu_bus_ptr->read_data();
                    break;
                case 6:
                    // Fetch the next background tile MSB bit plane from the pattern memory
                    // This is the same as above, but has a +8 offset to select the next bit plane
                    pattern_memory_selector = check_bit(_PPU_control_register, PPUCTRL_BG_PATTERN_TABLE_ADDR) << 12;
                    bg_next_tile_id_multiplied = _bg_next_tile_id << 4;
                    offset = 8;

                    _ppu_bus_ptr->set_address(pattern_memory_selector + bg_next_tile_id_multiplied + _current_vram_address.fine_y + offset);
                    _bg_next_tile_msb = _ppu_bus_ptr->read_data();
                    break;
                case 7:
                    // Increment the background tile "pointer" to the next tile horizontally
                    // in the nametable memory. Note this may cross nametable boundaries which
                    // is a little complex, but essential to implement scrolling
                    increment_scroll_x();
                    break;
            }
        }

        // End of a visible scanline, so increment downwards...
        if (_clock_pulse_x == 256) {
            increment_scroll_y();
        }

        //...and reset the x position, transferring the temp into vram
        if (_clock_pulse_x == 257) {
            if (bg_rendering_enabled()) {
                bg_load_shifters();
                _current_vram_address.nametable_x = _temp_vram_address.nametable_x;
                _current_vram_address.coarse_x    = _temp_vram_address.coarse_x;
            }

            if (_scanline_y >= 0) {
                // at clock 257, clear our sprite cache
                build_sprite_cache_next_scanline();
            }
        }

        if (_clock_pulse_x >= 257 && _clock_pulse_x <= 320) { // (the sprite tile loading interval)
            // OAM addr is set to 0 during each of ticks 257-320 
            _oam_addr = 0; 
        }

        // Superfluous reads of tile id at end of scanline
        if (_clock_pulse_x == 338 || _clock_pulse_x == 340) {
            _ppu_bus_ptr->set_address(0x2000 | (_current_vram_address.reg & 0x0FFF));
            _bg_next_tile_id = _ppu_bus_ptr->read_data();
        }

        if (_scanline_y == -1 && _clock_pulse_x >= 280 && _clock_pulse_x < 305 && bg_rendering_enabled()) {
            // End of vertical blank period so reset the Y address ready for rendering
            _current_vram_address.fine_y      = _temp_vram_address.fine_y;
            _current_vram_address.nametable_y = _temp_vram_address.nametable_y;
            _current_vram_address.coarse_y    = _temp_vram_address.coarse_y;
        }
    }

    /*if (_scanline_y == 240) {
        // nothing to do here... yet
    }*/

    if (_scanline_y >= 241 && _scanline_y < 261) {
        if (_scanline_y == 241 && _clock_pulse_x == 1) {
            vertical_blank();
        }
        // other cases incoming
    }

    // Composition - We now have background pixel information for this cycle
    // At this point we are only interested in background

    uint8_t bg_pixel = 0x00;   // The 2-bit pixel to be rendered
    uint8_t bg_palette = 0x00; // The 3-bit index of the palette the pixel indexes

    // We only render backgrounds if the PPU is enabled to do so. Note if
    // background rendering is disabled, the pixel and palette combine
    // to form 0x00. This will fall through the colour tables to yield
    // the current background colour in effect
    if (bg_rendering_enabled()) {
        // Handle Pixel Selection by selecting the relevant bit
        // depending upon fine x scolling. This has the effect of
        // offsetting ALL background rendering by a set number
        // of pixels, permitting smooth scrolling
        uint16_t bit_mux = 0x8000 >> _fine_x_scroll;

        // Select Plane pixels by extracting from the shifter
        // at the required location.
        uint8_t p0_pixel = (_bg_shifter_pattern_lo & bit_mux) > 0;
        uint8_t p1_pixel = (_bg_shifter_pattern_hi & bit_mux) > 0;

        // Combine to form pixel index
        bg_pixel = (p1_pixel << 1) | p0_pixel;

        // Get palette
        uint8_t bg_pal0 = (_bg_shifter_attrib_lo & bit_mux) > 0;
        uint8_t bg_pal1 = (_bg_shifter_attrib_hi & bit_mux) > 0;
        bg_palette = (bg_pal1 << 1) | bg_pal0;
    }

    // Now we have a final pixel colour, and a palette for this cycle
    // of the current scanline. Let's at long last, draw that ^&%*er :P
    // feed _result_pixel with the ID of the palette.

    _ppu_bus_ptr->set_address((0x3F00 + (bg_palette << 2) + bg_pixel));
    _result_pixel = _ppu_bus_ptr->read_data() & 0x3F;

    uint32_t pixel_index = (FRAME_WIDTH * 4 * _scanline_y) + ((_clock_pulse_x - 1) * 4);
    
    if (pixel_index + 3 <= FRAME_ARRAY_SIZE * 4) {
        _raw_pixel_data[pixel_index + 0] = NTSC_PALETTE[_result_pixel][B];      
        _raw_pixel_data[pixel_index + 1] = NTSC_PALETTE[_result_pixel][G];      
        _raw_pixel_data[pixel_index + 2] = NTSC_PALETTE[_result_pixel][R];              
        _raw_pixel_data[pixel_index + 3] = SDL_ALPHA_OPAQUE;      
    }

    // pulse the clocks
    _clock_pulse_x++;
    _ppu_system_clock++;

    if (_clock_pulse_x >= 341) {
        _clock_pulse_x = 0;
        _scanline_y++;

        if (_scanline_y >= 261) {
            _scanline_y = -1;
            _frame_count++; 
            _frame_complete_flag = true;        // indicate a completed frame (at least the visible portion)
        }
    }

    // lastly, manage the DMA event if that's what's happening at the time
    handle_dma();
}

void ppu::build_sprite_cache_next_scanline(void) {
    // start by clearing the sprite cache and resetting the count to zero
    for (auto& entry : _sprite_cache) {
        entry.y = 0xFF;
        entry.id = 0xFF;
        entry.attr = 0xFF;
        entry.x = 0xFF;                    
    }
    _sprite_count = 0;

    // seeing as the sprite count is now zero, clear the sprite overflow bit in case it was ever set
    _PPU_status_register &= ~(1 << PPUSTATUS_SPRITE_OVERFLOW);

    // Then check every sprite in the OAM to see which sprites are going to be visible on the next scanline
    for (uint8_t i = 0; i < MAX_OAM_SPRITES; i++) {
        int y_check = _scanline_y - _oam_data[i].y;

        if (y_check >= 0 && y_check < _sprite_height) { // are we within the boundaries of the sprites height?
            _sprite_cache[i] = _oam_data[i];

            _sprite_count++;

            if (_sprite_count >= MAX_SPRITES_SCANLINE) { // max this out at 8 sprites, much like the actual nes does
                _PPU_status_register |= (1 << PPUSTATUS_SPRITE_OVERFLOW);   // set the overflow bit
                break; // we can jump out of our for loop for now, we won't be rendering any more sprites
            }
        }
    }
}

void ppu::handle_dma(void) {
    // on to handling the DMA request
    if (_dma_transfer_status) {
        // on even clock cycles, read data
        if (_ppu_system_clock % 2 == 0) {
            uint16_t dma_abs_address = (_dma_page << 8) + _dma_addr;  // form a 16 bit address from the page and addr offset
            _cpu_bus_ptr->set_address(dma_abs_address);
            _dma_data = _cpu_bus_ptr->read_data();            
        }

        // then on odd clock cycles, write data
        if (_ppu_system_clock % 2 == 1) {
            _ptr_oam_data[_dma_addr] = _dma_data; // write the data
            _dma_addr++;    // and increment the dma addr            
        }

        // then when finished, continue the cpu execution
        if (_ppu_system_clock - _ppu_clock_at_start_of_dma >= 512) {    // once we've completed enough cycles to fill the 255 buffer, we can finish up.
            _cpu_ptr->DMA_continue();
            _dma_transfer_status = false;   

            // debug output
			/*for (uint8_t test_id = 0; test_id < 64; test_id++) {
				std::cout << "OAM " << std::hex << (uint16_t)test_id * 4 << 
					", y: " << (uint16_t)_ptr_oam_data[test_id * 4 + 0] << 
					", ID: " << (uint16_t)_ptr_oam_data[test_id * 4 + 1] << 
					", Attr: " << (uint16_t)_ptr_oam_data[test_id * 4 + 2] << 
					", x: " << (uint16_t)_ptr_oam_data[test_id * 4 + 3] <<
					std::dec << std::endl;
			}
			std::cout << std::endl << std::endl; */        
        } 
    }

    if (_dma_requested) {
        // we need to wait for an odd clock cycle to commence the DMA
        if (_ppu_system_clock % 2 == 1) {
            _ppu_clock_at_start_of_dma = _ppu_system_clock;     // for measuring how many cycles to wait for
            _dma_transfer_status = true;
            _dma_addr = 0;
            // the _dma_page is set within the write method, be sure not to overwrite it
            _cpu_ptr->DMA_suspend();        // suspend the CPU
            _dma_requested = false; // reset the DMA request bool so that we can move on
        }
    }    
}

void ppu::reset(void) {
    //reset statuses back to starting position
    _PPU_control_register = 0;
    _PPU_status_register = 0;
    _oam_addr = 0;
    
    // clear our oam data back to zero
    for (uint8_t i = 0; i < 255; i++) {
        _ptr_oam_data[i] = 0;
    }

	_dma_page = 0; 
    _dma_addr = 0; 
    _dma_data = 0;
	_dma_transfer_status = false; 
    _dma_requested = false;

    // set the palette to all black (0x3F, the final entry in the palette) If you do this before registering bus devices, this will do nothing
    for(uint8_t i = 0; i < PALETTE_RAM_SIZE; i++) {
        _ppu_bus_ptr->set_address(PALETTE_RAM_INDEX_START + i);
        _ppu_bus_ptr->write_data(0x3F);
    }

    _write_toggle = 0;
    _fine_x_scroll = 0;

    _current_vram_address.reg = 0; // reset the video memory address and temp vram too
    _temp_vram_address.reg = 0;

    // set the vertical blank bit to 1, indicating the PPU is busy
    _PPU_status_register |= (1 << PPUSTATUS_VERTICAL_BLANK);    

    _ppu_system_clock = 0;
    _ppu_clock_at_start_of_dma = 0;

    _scanline_y = 0;
    _clock_pulse_x = 0;

    _sprite_height = 8; // we'll default to 8x height for safety, updating PPUCTRL will overwrite this

    _frame_count = 0;
    _frame_complete_flag = false;

    for (auto& i: _raw_pixel_data) i = 0;   // clear the pixel data

    // reset our ppu cached variables
	_nametable_x = 0; 
    _nametable_y = 0; 
    _result_pixel = 0;

    _dma_transfer_status = false; 
}

void ppu::trigger_cpu_NMI(void) {
    _cpu_ptr->NMI();
}

uint8_t ppu::read(uint16_t address) {
    // the PPU ports are mirrored every 8 bytes, except for the OAMDMA
    if (address != OAMDMA) {
        address &= PPU_ADDRESS_SPACE_END;  
    }

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
            data = _ptr_oam_data[_oam_addr];
            break;
        case PPUDATA:
            data = _buffered_read;
            _ppu_bus_ptr->set_address(_current_vram_address.reg); 

            // ppu reads are behind by one cycle unless reading from palette memory  
            if (_current_vram_address.reg >= PALETTE_RAM_INDEX_START && _current_vram_address.reg <= PALETTE_RAM_MIRROR_END)
            {
                data = _ppu_bus_ptr->read_data();  // read directly
            }
            else {    
                data = _buffered_read;     // read out what was read last cycle
                _buffered_read = _ppu_bus_ptr->read_data();   // buffer whatever was read last
            }

            increment_video_memory_address();           
            break;           
    }  
    return data;
}

void ppu::write(uint16_t address, uint8_t data) {
    // the PPU ports are mirrored every 8 bytes, except for the OAMDMA
    if (address != OAMDMA) {
        address &= PPU_ADDRESS_SPACE_END;  
    }

    switch (address) { 
        // not all of these ports have write access  
        case PPUCTRL:
            _PPU_control_register = data & 0xFC;             // Lop off the bottom two bits as they are not used in this register
            _temp_vram_address.nametable_x = check_bit(data, 0);
            _temp_vram_address.nametable_y = check_bit(data, 1);
            _sprite_height = (check_bit(_PPU_control_register, PPUCTRL_SPRITE_SIZE) == 0 ? 8 : 16); // update the sprite width
            break;
        case PPUMASK:
            _PPU_mask_register = data;
            break;            
        case OAMADDR:
            _oam_addr = data;
            break;            
        case OAMDATA:
            _ptr_oam_data[_oam_addr] = data;
            break;     
        case PPUSCROLL: 
            if (_write_toggle == 0) {
                // on first write, set the course/fine x position
                _temp_vram_address.coarse_x = (data >> 3);      // top 5 bits becomes course x
                _fine_x_scroll = (data & 0x07);                 // bottom 3 bits become fine x
                _write_toggle = 1;          
            }
            else if (_write_toggle == 1) {
                // second write set the course/fine y position
                _temp_vram_address.coarse_y = (data >> 3);      // top 5 bits becomes course y
                _temp_vram_address.fine_y = (data & 0x07);      // bottom 3 bits becomes fine y
                _write_toggle = 0;
            }
            break;
        case PPUADDR:
            if (_write_toggle == 0) {
                // as per PPU Scrolling behaviour, certain bits need to be clipped off for this to function accurately. see this guide: https://wiki.nesdev.com/w/index.php/PPU_scrolling
                uint16_t clipped_data = (data & 0x3F) << 8; // clip the top two bits off and move into position
                _temp_vram_address.reg = clipped_data | (_temp_vram_address.reg & 0x00FF);     // pretty much a copy of both
                _temp_vram_address.unused = 0;              // our z bit is cleared. Unused so this does nothing.
                _write_toggle = 1;          
            } 
            else if (_write_toggle == 1) {
                _temp_vram_address.reg = (_temp_vram_address.reg & 0xFF00) | data;      // just the top 8 bits
                _current_vram_address.reg = _temp_vram_address.reg;                     // copy tram over to vram
                _write_toggle = 0;
            }
            break;              
        case PPUDATA:
            _ppu_bus_ptr->set_address(_current_vram_address.reg); // for safety we may want to chop off the top two bits of the address
            _ppu_bus_ptr->write_data(data);     
            increment_video_memory_address();
            break;  	
        case OAMDMA:
            // trigger the DMA event
            _dma_page = data;
            _dma_requested = true;
            break;          
    }  
}

uint16_t ppu::get_video_memory_address(void) {
    return _current_vram_address.reg;
}

bool ppu::get_vertical_blank(void) {
    return check_bit(_PPU_status_register, 7);
}

void ppu::vertical_blank(void) {    
    // set the vertical blank bit in the Status register, indicating to the rest of the system that we are okay to start writing pixel data
    _PPU_status_register |= (1 << PPUSTATUS_VERTICAL_BLANK);

    // trigger the NMI on Vblank if that PPUCTRL register was set
    if (check_bit(_PPU_control_register, PPUCTRL_VERTICAL_BLANK_NMI) == 1) {
        trigger_cpu_NMI(); 
    }
}

void ppu::increment_video_memory_address(void) {
    // PPUCTRL register is read, and bit 3 determines if we increment by 1 (incrementing x) or 32 (incrementing our y)
    _current_vram_address.reg += (check_bit(_PPU_control_register, PPUCTRL_VRAM_INCREMENT) == 0 ? 1 : 32);
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

oam_entry ppu::debug_read_oam(uint8_t relative_address) {
    return _oam_data[relative_address];
}

uint8_t ppu::debug_read_oam_ptr(uint8_t relative_address) {
    return _ptr_oam_data[relative_address];
}