#include "bus_device.h"

bus_device::bus_device(bus* bus_ptr) {
    _bus_ptr = bus_ptr;

    // bind storage for our read and write pointers
    _read_function_ptr = std::bind(&bus_device::read, this, 0);
    _write_function_ptr = std::bind(&bus_device::write, this, 0, 0);     
}