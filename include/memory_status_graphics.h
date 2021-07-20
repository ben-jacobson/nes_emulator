#pragma once

#include "status_graphics.h"
#include "ram.h"
#include <iomanip>

class ram_status_output : public emulator_status_graphics
{
public:
	ram_status_output(SDL_Renderer* renderer, const char* font_filename, int ptsize, ram* ram_ptr);
	~ram_status_output() = default;
    
    void display_ram_contents(uint16_t x, uint16_t y);

    uint16_t _start_address = 0; // allows you to display different sections of ram, e.g you could theoretically have two of these display different sections of ram. 

private:
    constexpr static uint8_t BYTES_TO_DISPLAY = 8 * 16; // we only show the first 128 bytes of RAM, essentially creating an 8x16 matrix. 
    ram* _ram_ptr; 
};


