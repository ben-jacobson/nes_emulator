#include "instr_trace_graphics.h"

instr_trace_graphics::instr_trace_graphics(instruction_log* log_ptr, SDL_Renderer* renderer, const char* font_filename, int ptsize, uint16_t preset_display_x, uint16_t preset_display_y)
    : status_graphics(renderer, font_filename, ptsize)
{
    _log_ptr = log_ptr;
    _preset_display_x = preset_display_x;
    _preset_display_y = preset_display_y;
}

void instr_trace_graphics::display_contents() {
    set_colour({0, 255, 0, 255}); // green
    draw_to_buffer(">", _preset_display_x, _preset_display_y + (_font_height * 4));

    set_colour({255, 255, 255, 255}); // green
    draw_to_buffer("Instruction Trace", _preset_display_x + 20, _preset_display_y);

    for (uint8_t i = 0; i < _log_ptr->INSTRUCTION_COUNT; i++) {
        draw_to_buffer(_log_ptr->_instruction_trace[i], _preset_display_x + 20, _preset_display_y + ((i + 1) * _font_height));
    }
}