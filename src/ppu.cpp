#include "ppu.h"

ppu::ppu(bus* cpu_bus_ptr, bus* ppu_bus_ptr, cpu* cpu_ptr) {
    _cpu_bus_ptr = cpu_bus_ptr;
    _ppu_bus_ptr = ppu_bus_ptr;
    _cpu_ptr = cpu_ptr;

    reset();
}

void ppu::cycle(void) {
    /*
        The CPU works by taking an instruction from the byte read from data. However the PPU works differently
        The CPU interfaces with the PPU simply by reading or writing to it's address range.
        The first byte is the port, and the second byte being the instruction

        we use this cycle method to update the internal status, and handle all of the heavy lifting, but using the read/write functions allow 
        to interface with it's memory
    */ 

   // just some temporary code to get us through. set the vertical blank
   _PPU_status_register = (1 << 7); 
}

void ppu::reset(void) {
    //reset statuses back to starting position
    _PPU_control_register = 0;
    _PPU_status_register = 0;
    _PPU_oam_data_status_register = 0;
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
            data = _PPU_status_register; 
            break;
        case OAMDATA:
            data = _PPU_oam_data_status_register;
            break;
        case PPUDATA:
            data = _ppu_bus_ptr->read_data();
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
            // this requires two writes to work.
            _PPU_addr_register = _PPU_addr_register << 8;
            _PPU_addr_register |= data;
            _ppu_bus_ptr->set_address(_PPU_addr_register);  //unsure if setting the address when only 1/2 of the address is gathered will cause a bug or not.  
            break;              
        case PPUDATA:
            _ppu_bus_ptr->write_data(data);
            break;            
    }  
}