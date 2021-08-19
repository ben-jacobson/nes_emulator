#include "apu_io.h"

apu_io::apu_io(bus* bus_ptr, uint16_t address_space_lower, uint16_t address_space_upper)
    : bus_device()
{
    _bus_ptr = bus_ptr;
    _address_space_lower = address_space_lower;
    _address_space_upper = address_space_upper;    
}

uint8_t apu_io::read(uint16_t address) {
    return 0;
}

void apu_io::write(uint16_t address, uint8_t data) {

}

void apu_io::process_key(uint8_t key, uint8_t state) {
    
}