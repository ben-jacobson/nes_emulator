#pragma once

#include "bus.h"
#include <functional>

class bus_device    
{
public:
    uint16_t _address_space_lower, _address_space_upper; // define at which point the device "listens" to an address. This is optional, e.g the CPU doesn't need to be part of the address space, but the RAM is. allowing devices up to 64K means these need to be 16bit uints.

    bus_device(bus *bus_ptr);
    ~bus_device() = default;

    virtual uint8_t read(uint16_t address) = 0;
    virtual void write(uint16_t address, uint8_t data) = 0;

protected: 
    bus* _bus_ptr = nullptr;  
};
