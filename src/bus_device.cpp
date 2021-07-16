#include "bus_device.h"

bus_device::bus_device(bus* bus_ptr) {
    _bus_ptr = bus_ptr;
    // note that the constructor does nothing with the read/write function pointers, it's up to each child class / device type to implement what these do, e.g a ROM has no write method, but RAM has both read and write.
}