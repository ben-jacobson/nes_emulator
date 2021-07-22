#pragma once

#include "status_graphics.h"
#include "bus.h"
#include <iomanip>

class memory_status_graphics : public status_graphics
{
public:
	memory_status_graphics(bus* bus_ptr, SDL_Renderer* renderer, const char* font_filename, int ptsize, std::string display_heading, uint16_t start_address);
	memory_status_graphics(bus* bus_ptr, SDL_Renderer* renderer, const char* font_filename, int ptsize, uint16_t preset_display_x, uint16_t preset_display_y, std::string display_heading, uint16_t start_address);    
	~memory_status_graphics() = default;
    
    void display_contents(uint16_t x, uint16_t y);
    void display_contents(void);

    uint16_t _start_address = 0; // allows you to display different sections of ram, e.g you could theoretically have two of these display different sections of ram. 

private:
    constexpr static uint8_t BYTES_TO_DISPLAY = 8 * 16; // we only show the first 128 bytes of RAM, essentially creating an 8x16 matrix. 
    const uint16_t _START_ADDRESS;  // Which address do we want to start from?  
    std::string _display_heading; 
    uint16_t _preset_display_x, _preset_display_y; 
    bus* _bus_ptr;
};
