#include "bus_device.h"

bus_device::bus_device() {
    // note that the constructor does nothing with the read/write function pointers, it's up to each child class / device type to implement what these do, e.g a ROM has no write method, but RAM has both read and write.

    // set up function pointers, binding this to the virtual function will mean every derived class will have their function pointers set up automatically. 
	_read_function_ptr = std::bind(&bus_device::read, this, std::placeholders::_1);
    _write_function_ptr = std::bind(&bus_device::write, this, std::placeholders::_1, std::placeholders::_2); 
}

bus_device::~bus_device() {
    // nothing to do!
}

uint8_t check_bit(uint8_t data, uint8_t bit) {
	return ((data & (1 << bit)) >> bit) == 1 ? 1 : 0;
}

uint8_t flip_byte(uint8_t data) {
    // thanks for the quick and easy byte flip: https://stackoverflow.com/questions/2602823/
    uint8_t new_byte = data; 
    new_byte = (new_byte & 0xF0) >> 4 | (new_byte & 0x0F) << 4;
    new_byte = (new_byte & 0xCC) >> 2 | (new_byte & 0x33) << 2;
    new_byte = (new_byte & 0xAA) >> 1 | (new_byte & 0x55) << 1;
    return new_byte;    
}