#include "cartridge.h"

cartridge::cartridge(bus* bus_ptr, uint16_t address_space_lower, uint16_t address_space_upper) 
:   bus_device(bus_ptr)
{
    // disable the write function pointer
    _write_function_ptr = nullptr;

    // temporarily set the address space boundaries to zero in this implementation
    _address_space_lower = address_space_lower;
    _address_space_upper = address_space_upper;   

    _rom_data = new uint8_t[ROM_SIZE_BYTES]; // allocate the ROM within heap memory to a specified size
    // I'm happy to just use a C style array, the needs of this are really simple. 

    // initialize the ram content to all zeros
    for (uint16_t i = 0; i < ROM_SIZE_BYTES; i++) {
        _rom_data[i] = 0; // initialize rom with all zeros
    }
}

cartridge::~cartridge() {
    delete[] _rom_data; // free the memory. 
}

uint8_t cartridge::read(uint16_t address) {
    // First check if the read is within the specified address range
    if (address >= _address_space_lower && address <= _address_space_upper) {
        if (address >= ROM_ADDRESS_SPACE_START) { //  && address <= ROM_ADDRESS_SPACE_END) { // we won't check that is at end address because it it's the largest value a uint16_t can have
            return read_rom(address);
        }
        else {
            return 0; // TODO!!
        }
    }
    return 0;
}

void cartridge::write(uint16_t address, uint8_t data) {
    // do nothing, we cannot write to ROM
    address += data; // surpress warning 
    return;
}	

uint8_t cartridge::read_rom(uint16_t address) {
    // First check if the read is within the specified address range
    if (address >= ROM_ADDRESS_SPACE_START) { //   && address <= ROM_ADDRESS_SPACE_END) {  // we won't check that is at end address because it it's the largest value a uint16_t can have
        return _rom_data[address - ROM_ADDRESS_SPACE_START]; // new mapped address is offset by _address_space_lower;
    }
    return 0;
}


uint8_t cartridge::debug_read(uint16_t relative_address) {
    // notice there is no address space checking, we simply output whatever is at the relative address, e.g 0 is the start and MAX_SIZE is the end
    return _rom_data[relative_address];    
}

uint8_t* cartridge::get_rom_pointer(void) {
    return _rom_data;
}