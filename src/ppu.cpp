#include "ppu.h"

ppu::ppu(bus* cpu_bus_ptr, bus* ppu_bus_ptr, cpu* cpu_ptr) {
    _cpu_bus_ptr = cpu_bus_ptr;
    _ppu_bus_ptr = ppu_bus_ptr;
    _cpu_ptr = cpu_ptr;

    reset();
}

void ppu::cycle(void) {
    // draw pixel by pixel to output
    vertical_blank();
}

void ppu::reset(void) {
    //reset statuses back to starting position
    _PPU_control_register = 0;
    _PPU_status_register = 0;
    _PPU_oam_data_status_register = 0;

    _address_latch = false; 
    _addr_second_write = false; 
    _scroll_second_write = false; 

    _video_memory_address = 0; // reset the video memory address

    // set the vertical blank bit to 1, indicating the PPU is busy
    _PPU_status_register |= (1 << PPUSTATUS_VERTICAL_BLANK);
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
            _PPU_status_register &= ~(1 << PPUSTATUS_VERTICAL_BLANK); // clear the vertical blank on status reads
            _address_latch = true;  // allow PPUADDRESS and PPUSCROLL to start their 2x write process
            data = _PPU_status_register; 

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
                    _PPU_addr_register = _PPU_addr_register << 8;   // shift the upper 8 bits into position
                    _PPU_addr_register |= data;                     // read the lower 8 bits on the second read
                    _ppu_bus_ptr->set_address(_PPU_addr_register); 
                    _addr_second_write = false;         // reset back to default
                }
                else {
                    _PPU_addr_register = data;  // this reads the upper 8 bits
                    _addr_second_write = true;       // enable the second write
                } 
            }
            break;              
        case PPUDATA:
            _ppu_bus_ptr->write_data(data);     // we rely on setting the address via PPUADDR writes, it's possible to write junk data to this if you just go straight for the write
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
        trigger_cpu_NMI();
    }
}

void ppu::increment_video_memory_address(void) {
    // PPUCTRL register is read, and bit 3 determines if we increment by 1 (going x) or 32 (incrementing our y)
    _video_memory_address += (check_bit(_PPU_control_register, PPUCTRL_VRAM_INCREMENT) == 0 ? 1 : 32);
}