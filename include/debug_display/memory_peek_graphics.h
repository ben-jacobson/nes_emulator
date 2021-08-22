#pragma once

#include "text_renderer.h"
#include "bus.h"
#include <iomanip>

class memory_peek_graphics
{
public:
	memory_peek_graphics(text_renderer* text_surface, bus* bus_ptr, std::string headline, uint16_t preset_display_x, uint16_t preset_display_y);
	~memory_peek_graphics() = default;

    void activate_cursor(void);
    void display_contents(void);
	void set_address(uint16_t address);
    void input_partial_address(char input);

private:
	text_renderer* _text_surface;

    bool _cursor_active; 
    uint16_t _preset_display_x, _preset_display_y; 
    uint16_t _address;
    std::string _partial_address;
    std::string _headline; 
    bus* _bus_ptr;	
};
