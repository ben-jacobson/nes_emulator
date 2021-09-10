#pragma once

#include "bus.h"
#include <functional>

class bus_device    
{
public:
    uint16_t _address_space_lower, _address_space_upper; // define at which point the device "listens" to an address. This is optional, e.g the CPU doesn't need to be part of the address space, but the RAM is. allowing devices up to 64K means these need to be 16bit uints.

    bus_device();
    ~bus_device();

    // function pointers for read and write. Basically the bus maintains a list of these so that when it decodes the address, it knows which device to trigger for read/write operations
	std::function<uint8_t(uint16_t)> _read_function_ptr = nullptr;
	std::function<void(uint16_t, uint8_t)> _write_function_ptr = nullptr;    

    virtual uint8_t read(uint16_t address) = 0;     // this is a pure virtual class now
    virtual void write(uint16_t address, uint8_t data) = 0;
};

uint8_t check_bit(uint8_t data, uint8_t bit);
uint8_t flip_byte(uint8_t data);