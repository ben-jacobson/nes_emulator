#include "processor_status_graphics.h"

processor_status_graphics::processor_status_graphics(cpu* cpu_ptr, SDL_Renderer* renderer, const char* font_filename, int ptsize, uint16_t preset_display_x, uint16_t preset_display_y)
    : status_graphics(renderer, font_filename, ptsize)
{
    _cpu_ptr = cpu_ptr;
    _preset_display_x = preset_display_x;
    _preset_display_y = preset_display_y;
}

void processor_status_graphics::display_contents(void) {
    /*
    set_colour({0, 255, 0, 255}); // green
    draw_to_buffer(">", _preset_display_x, _preset_display_y + (_font_height * INSTRUCTIONS_TO_DISPLAY / 2)); // should be 6 down

    set_colour({255, 255, 255, 255}); // green
    draw_to_buffer("Instruction Trace", _preset_display_x + 20, _preset_display_y);

    std::string decoded_instruction;
    //_current_address = _start_address - (INSTRUCTIONS_TO_DISPLAY / 2) + 1; // reset back to the start of where to view the trace.
    _current_address = _cpu_ptr->get_program_counter() - (INSTRUCTIONS_TO_DISPLAY / 2) + 1; // reset back to the start of where to view the trace.

    for (uint8_t i = 0; i < INSTRUCTIONS_TO_DISPLAY; i++) {
        _bus_ptr->set_address(_current_address);
        fetch_and_decode_next_instruction();
        draw_to_buffer(_decoded_instruction, _preset_display_x + 20, _preset_display_y + ((i + 1) * _font_height));
    }
    */
    uint8_t flags = _cpu_ptr->get_status_flags();
    uint8_t flag_bit = 0;

    draw_to_buffer("Processor Status", _preset_display_x, _preset_display_y);
    draw_to_buffer("C Z I D B U V N", _preset_display_x, _preset_display_y + _font_height);
    
    //std::cout << "processor flags:" << (uint16_t)flags << std::endl;
    
    for (uint8_t i = 8; i > 0; i--) {
        flag_bit = (flags >> (i-1) & 1);
        flag_bit == 1 ? set_colour({0, 255, 0, 255}) : set_colour({255, 255, 255, 255}); // set colour based on bit value
        draw_to_buffer(flag_bit == 1 ? "1 " : "0 ", _preset_display_x + (16 * _font_width) - (_font_width * 2 * i), _preset_display_y + (_font_height * 2));
        //std::cout << (uint16_t)(i-1) << ": " << (uint16_t)flag_bit << std::endl;
    }

    //Render a view of the registers - ACC, X, Y. Also showing the stack pointer, program counter and the number of cycles elapsed since reset
}
