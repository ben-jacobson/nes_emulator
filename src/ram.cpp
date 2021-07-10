#include "ram.h"

ram::ram(bus* bus_ptr) 
:   bus_device(bus_ptr)
{

}

void ram::read(void) {
    // check if the read is within the specified address range
    // write this to the data bus via it's pointer
}

void ram::write(void) {

}
