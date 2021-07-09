#pragma once

#include "bus.h"

class bus_device
{
    public:
        bus_device(bus* bus_ptr);
        ~bus_device() = default;

    protected: 
        bus* _bus_ptr = nullptr;
        uint16_t address_space_lower, address_space_upper; // define at which point the device "listens" to an address. This is optional, e.g the CPU doesn't need to be part of the address space, but the RAM is. allowing devices up to 64K means these need to be 16bit uints.
};
