#include "instr_trace_graphics.h"

instr_trace_graphics::instr_trace_graphics(text_renderer* text_surface, instruction_log* log_ptr, uint16_t preset_display_x, uint16_t preset_display_y) {
    _log_ptr = log_ptr;
    _preset_display_x = preset_display_x;
    _preset_display_y = preset_display_y;

    _text_surface = text_surface;
}

void instr_trace_graphics::display_contents() {
    _text_surface->set_colour({0, 255, 0, 255}); // green
    _text_surface->draw_char('>', _preset_display_x, _preset_display_y + (_text_surface->_font_size * 4));

    _text_surface->set_colour({255, 255, 255, 255}); // green
    _text_surface->draw_text("Instruction Trace", _preset_display_x + 20, _preset_display_y);

    for (uint8_t i = 0; i < _log_ptr->INSTRUCTION_COUNT; i++) {
        _text_surface->draw_text(_log_ptr->_instruction_trace[i], _preset_display_x + 20, _preset_display_y + ((i + 1) * _text_surface->_font_size));
    }
}