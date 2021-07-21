#include "instr_trace_graphics.h"

instr_trace_graphics::instr_trace_graphics(bus* bus_ptr, uint16_t start_address, SDL_Renderer* renderer, const char* font_filename, int ptsize, uint16_t preset_display_x, uint16_t preset_display_y)
    : status_graphics(renderer, font_filename, ptsize)
{
    _bus_ptr = bus_ptr;
    
    _preset_display_x = preset_display_x;
    _preset_display_y = preset_display_y;

    _start_address = start_address;
}

void instr_trace_graphics::display_contents() {
    set_colour({0, 255, 0, 255}); // green
    draw_to_buffer(">", _preset_display_x, _preset_display_y + (_font_height * INSTRUCTIONS_TO_DISPLAY / 2)); // should be 6 down

    set_colour({255, 255, 255, 255}); // green
    draw_to_buffer("Instruction Trace", _preset_display_x + 20, _preset_display_y);

    for (uint8_t i = 0; i < INSTRUCTIONS_TO_DISPLAY; i++) {
        _bus_ptr->set_address(_start_address + i);
        draw_to_buffer("0x8000: 0xBB 0xAA 0xFF. INDX: STX 126 50", _preset_display_x + 20, _preset_display_y + ((i + 1) * _font_height));
    }
}
