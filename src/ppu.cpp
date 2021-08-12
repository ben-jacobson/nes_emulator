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

}

void ppu::reset(void) {
    _instruction_ready = true; 
    _latched_address = false; 

    //reset statuses back to starting position
    _ppu_status = 0;
}

void ppu::trigger_cpu_NMI(void) {
    _cpu_ptr->NMI();
}

uint8_t ppu::read(uint16_t address) {
    switch (address) { 
        case PPUCTRL:
            break;
        case PPUMASK:
            break;
        case PPUSTATUS:
            return 0xEE; // temporary
            break;
        case OAMADDR:
            break;
        case OAMDATA:
            break;
        case PPUSCROLL:
            break;
        case PPUADDR:
            break;
        case PPUDATA:
            break;
        case OAMDMA:
            break;	
    }  
    return 0;
}

void ppu::write(uint16_t address, uint8_t data) {
    switch (address) { 
        case PPUCTRL:
            break;
        case PPUMASK:
            break;
        case PPUSTATUS:
            _ppu_status = data;
            break;
        case OAMADDR:
            break;
        case OAMDATA:
            break;
        case PPUSCROLL:
            break;
        case PPUADDR:
            break;
        case PPUDATA:
            break;
        case OAMDMA:
            break;	
    }  
}