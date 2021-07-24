#pragma once

#include "status_graphics.h"
#include "bus.h"
#include <iomanip>

class memory_peek_graphics : public status_graphics
{
public:
	memory_peek_graphics(bus* bus_ptr, SDL_Renderer* renderer, const char* font_filename, int ptsize, uint16_t preset_display_x, uint16_t preset_display_y);
	~memory_peek_graphics() = default;

    void display_contents(void);
	void set_address(uint16_t address);

private:
    uint16_t _preset_display_x, _preset_display_y; 
    uint16_t _address;
    bus* _bus_ptr;	
};
