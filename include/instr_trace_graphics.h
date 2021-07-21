#pragma once

#include <SDL2/SDL_ttf.h>
#include "status_graphics.h"
#include "bus.h"
#include <iomanip>


class instr_trace_graphics : public status_graphics
{
public:
	instr_trace_graphics(bus* bus_ptr, uint16_t start_address, SDL_Renderer* renderer, const char* font_filename, int ptsize, uint16_t preset_display_x, uint16_t preset_display_y);
	~instr_trace_graphics() = default;

    void display_contents();

private:
    static constexpr uint8_t INSTRUCTIONS_TO_DISPLAY = 10;
    uint16_t _start_address;
    bus* _bus_ptr;
    uint16_t _preset_display_x, _preset_display_y; 
};
