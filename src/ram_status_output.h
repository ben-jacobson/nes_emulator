#pragma once
#include "emulator_status_graphics.h"

class ram_status_output : public emulator_status_graphics
{
public:
	ram_status_output();
	~ram_status_output() = default;

private:
    uint8_t* _ram_ptr;
    const uint16_t _ram_size; 
};
