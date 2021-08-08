#include "ppu.h"

ppu::ppu(bus* cpu_bus_ptr, bus* ppu_bus_ptr) {
    _cpu_bus_ptr = cpu_bus_ptr;
    _ppu_bus_ptr = ppu_bus_ptr;
}

// being a bus device, we need to define these, they refer to read and write activity on the main cpu bus
uint8_t ppu::read(uint16_t address) {
    switch (address) {
        case PPUCTRL:
        break;
        case PPUMASK:
        break;
        case PPUSTATUS:
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

void ppu::write(uint16_t address, uint8_t data) {
    // The PPU can change it's state just by being written to, we interface with it via it's 8 ports
    switch (address) {
        case PPUCTRL:
        break;
        case PPUMASK:
        break;
        case PPUSTATUS:
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

uint8_t ppu::PPUread(uint16_t address) {

}

void ppu::PPUwrite(uint16_t address, uint8_t data) {
 
}	   