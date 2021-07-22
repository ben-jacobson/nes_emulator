#include "processor_status_graphics.h"

processor_status_graphics::processor_status_graphics(cpu* cpu_ptr, SDL_Renderer* renderer, const char* font_filename, int ptsize, uint16_t preset_display_x, uint16_t preset_display_y)
    : status_graphics(renderer, font_filename, ptsize)
{
    _cpu_ptr = cpu_ptr;
    _preset_display_x = preset_display_x;
    _preset_display_y = preset_display_y;
}

void processor_status_graphics::display_contents(void) {
    uint8_t flags = _cpu_ptr->get_status_flags();
    uint8_t flag_bit = 0;

    draw_to_buffer("Processor Status", _preset_display_x, _preset_display_y);
    draw_to_buffer("N V U B D I Z C", _preset_display_x, _preset_display_y + _font_height);
    //std::cout << "processor flags:" << (uint16_t)flags << std::endl;
    
    for (uint8_t i = 8; i > 0; i--) {
        flag_bit = (flags >> (i-1) & 1);
        flag_bit == 1 ? set_colour({0, 255, 0, 255}) : set_colour({255, 255, 255, 255}); // set colour based on bit value
        draw_to_buffer(flag_bit == 1 ? "1 " : "0 ", _preset_display_x + (16 * _font_width) - (_font_width * 2 * i), _preset_display_y + (_font_height * 2));
        //std::cout << (uint16_t)(i-1) << ": " << (uint16_t)flag_bit << std::endl;
    }

    //Render a view of the registers - ACC, X, Y. Also showing the stack pointer, program counter and the number of cycles elapsed since reset
    std::stringstream register_values;
    register_values << "PC: " << "0x" << std::setfill('0') << std::setw(4) << std::uppercase << std::hex << _cpu_ptr->get_program_counter() << " ";
    register_values << "SP: " << "0x" << std::setfill('0') << std::setw(2) << std::uppercase << std::hex << (uint16_t)_cpu_ptr->get_stack_pointer() << " ";
    set_colour({255, 255, 255, 255}); // White
    draw_to_buffer(register_values.str(), _preset_display_x, _preset_display_y + (_font_height * 4));

    register_values = std::stringstream();
    uint8_t acc_reg = _cpu_ptr->get_accumulator_reg_content();
    uint8_t x_reg = _cpu_ptr->get_x_index_reg_content();
    uint8_t y_reg = _cpu_ptr->get_y_index_reg_content();

    register_values << "ACC: " << "0x" << std::setfill('0') << std::setw(2) << std::uppercase << std::hex << (uint16_t)acc_reg << " ";
    register_values << "(" << (uint16_t)acc_reg << ") ";
    register_values << "X: " << "0x" << std::setfill('0') << std::setw(2) << std::uppercase << std::hex << (uint16_t)x_reg << " ";
    register_values << "(" << (uint16_t)x_reg << ") ";
    register_values << "Y: " << "0x" << std::setfill('0') << std::setw(2) << std::uppercase << std::hex << (uint16_t)y_reg << " ";
    register_values << "(" << (uint16_t)y_reg << ") ";
    draw_to_buffer(register_values.str(), _preset_display_x, _preset_display_y + (_font_height * 5));

    register_values = std::stringstream();
    register_values << "Cycles: " << _cpu_ptr->debug_get_cycle_count();
    draw_to_buffer(register_values.str(), _preset_display_x, _preset_display_y + (_font_height * 6));
}
