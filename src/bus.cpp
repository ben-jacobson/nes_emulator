#include "bus.h"

void bus::set_address(uint16_t address) {
    _address = address;
}

uint16_t bus::read_address(void) {
    return _address;
}

void bus::write_data(uint8_t data) {
    _data = data;
}

uint8_t bus::read_data(void) {
    return _data;
}

unsigned int Factorial(unsigned int number) {
    return number <= 1 ? number : Factorial(number-1)*number;
}
