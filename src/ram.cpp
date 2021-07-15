#include "ram.h"

ram::ram(bus* bus_ptr, uint16_t ram_size, uint16_t address_space_lower, uint16_t address_space_upper) 
:   bus_device(bus_ptr)
{
    // temporarily set the address space boundaries to zero in this implementation
    _address_space_lower = address_space_lower;
    _address_space_upper = address_space_upper;    

    _ram_data = new uint8_t[ram_size]; // allocate the RAM within heap memory to a specified size
    // I'm happy to just use a C style array, the needs of this are really simple. 

    // initialize the ram content to all zeros
    for (uint16_t i = 0; i < ram_size; i++) {
        _ram_data[i] = 0;
    }
}

ram::~ram() {
    delete[] _ram_data; // free the memory. 
}

uint8_t ram::debug_read(uint16_t relative_address) {
    // notice there is no address space checking, we simply output whatever is at the relative address, e.g 0 is the start and MAX_SIZE is the end
    return _ram_data[relative_address];
}

void ram::read(void) {
    uint16_t address = _bus_ptr->read_address();

    // First check if the read is within the specified address range
    if (address >= _address_space_lower && address <= _address_space_upper) {
        uint8_t data = _ram_data[address - _address_space_lower]; // new mapped address is offset by _address_space_lower;
        _bus_ptr->write_data(data); // write this data to the bus
    }
}

void ram::write(void) {
    uint16_t address = _bus_ptr->read_address();

    // First check if the read is within the specified address range
    if (address >= _address_space_lower && address <= _address_space_upper) {
        uint8_t data = _bus_ptr->read_data(); // take the data from the bus
        _ram_data[address - _address_space_lower] = data;  // place it into the array at the new offset address
    }
}

uint8_t* ram::get_ram_pointer(void) {
    return _ram_data;
}
