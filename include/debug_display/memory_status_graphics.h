#pragma once

#include "text_renderer.h"
#include "bus.h"
#include <iomanip>

class memory_status_graphics
{
public:
	memory_status_graphics(text_renderer* text_surface, bus* bus_ptr, uint16_t preset_display_x, uint16_t preset_display_y, std::string display_heading, uint16_t start_address);    
	~memory_status_graphics() = default;
    
    void display_contents(uint16_t x, uint16_t y);
    void display_contents(void);

    uint16_t _start_address = 0; // allows you to display different sections of ram, e.g you could theoretically have two of these display different sections of ram. 

private:
	text_renderer* _text_surface;

    constexpr static uint8_t BYTES_TO_DISPLAY = 8 * 8; // we only show the first 64 bytes of memory, essentially creating an 8x8 matrix. 
    const uint16_t _START_ADDRESS;  // Which address do we want to start from?  
    std::string _display_heading; 
    uint16_t _preset_display_x, _preset_display_y; 
    bus* _bus_ptr;
};
