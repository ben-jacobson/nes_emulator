#include "cartridge.h"

#include <iostream>

cartridge::cartridge(bus* bus_ptr, uint16_t address_space_lower, uint16_t address_space_upper) 
:   bus_device(bus_ptr)
{
    // temporarily set the address space boundaries to zero in this implementation
    _address_space_lower = address_space_lower;
    _address_space_upper = address_space_upper;    

    _rom_data = new uint8_t[ROM_SIZE_BYTES]; // allocate the ROM within heap memory to a specified size
    // I'm happy to just use a C style array, the needs of this are really simple. 

    // initialize the ram content to all zeros
    for (uint16_t i = 0; i < ROM_SIZE_BYTES; i++) {
        _rom_data[i] = i; // test data for now
    }
}

cartridge::~cartridge() {
    delete[] _rom_data; // free the memory. 
}

void cartridge::read(void) {
    uint16_t address = _bus_ptr->read_address();

    // First check if the read is within the specified address range
    if (address >= _address_space_lower && address <= _address_space_upper) {
        uint8_t data = _rom_data[address - _address_space_lower]; // new mapped address is offset by _address_space_lower;
        _bus_ptr->write_data(data); // write this data to the bus
    }
}

uint8_t cartridge::debug_read(uint16_t relative_address) {
    // notice there is no address space checking, we simply output whatever is at the relative address, e.g 0 is the start and MAX_SIZE is the end
    return _rom_data[relative_address];    
}

uint8_t* cartridge::get_rom_pointer(void) {
    return _rom_data;
}