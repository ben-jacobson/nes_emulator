#include "ram.h"

ram::ram(bus* bus_ptr, uint16_t ram_size, uint16_t address_space_lower, uint16_t address_space_upper) 
:   bus_device(bus_ptr)
{
    // temporarily set the address space boundaries to zero in this implementation
    _address_space_lower = address_space_lower;
    _address_space_upper = address_space_upper;    

    _ram_size = ram_size;
    _ram_data = new uint8_t[_ram_size]; // allocate the RAM within heap memory to a specified size
    // I'm happy to just use a C style array, the needs of this are really simple. 

    clear_ram();   
}

ram::~ram() {
    delete[] _ram_data; // free the memory. 
}

uint8_t ram::debug_read(uint16_t relative_address) {
    // notice there is no address space checking, we simply output whatever is at the relative address, e.g 0 is the start and MAX_SIZE is the end
    return _ram_data[relative_address];
}

uint8_t* ram::get_ram_pointer(void) {
    return _ram_data;
}

uint8_t ram::read(uint16_t address) {
    // First check if the read is within the specified address range
    if (address >= _address_space_lower && address <= _address_space_upper) {
        return _ram_data[address - _address_space_lower]; // new mapped address is offset by _address_space_lower;
    }
    return 0;
}

void ram::write(uint16_t address, uint8_t data) {
    // First check if the read is within the specified address range
    if (address >= _address_space_lower && address <= _address_space_upper) {
        _ram_data[address - _address_space_lower] = data;  // place it into the array at the new offset address
    }
}

void ram::clear_ram(void) {
    // initialize the ram content to all 0x00's
    for (uint16_t i = 0; i < _ram_size; i++) {
        _ram_data[i] = 0;
    }
}
    