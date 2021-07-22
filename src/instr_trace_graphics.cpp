#include "instr_trace_graphics.h"

instr_trace_graphics::instr_trace_graphics(cpu* cpu_ptr, bus* bus_ptr, SDL_Renderer* renderer, const char* font_filename, int ptsize, uint16_t preset_display_x, uint16_t preset_display_y)
    : status_graphics(renderer, font_filename, ptsize)
{
    _bus_ptr = bus_ptr;
    _cpu_ptr = cpu_ptr;
    
    _preset_display_x = preset_display_x;
    _preset_display_y = preset_display_y;
}

void instr_trace_graphics::display_contents() {
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
}

void instr_trace_graphics::fetch_and_decode_next_instruction(void) {
    uint8_t opcode = _bus_ptr->read_data(); // we are already at the address
    std::string instruction_name = _cpu_ptr->_opcode_decoder_lookup[opcode].name;
    std::string address_mode = _cpu_ptr->_opcode_decoder_lookup[opcode].address_mode_name;
    uint8_t instruction_bytes = _cpu_ptr->_opcode_decoder_lookup[opcode].instruction_bytes;
    uint8_t operand; 
    
    std::stringstream decoded_line;
    decoded_line << "0x" << std::setfill('0') << std::setw(4) << std::uppercase << std::hex << _current_address << ": ";
    decoded_line << "0x" << std::setfill('0') << std::setw(2) << std::uppercase << std::hex << (uint16_t)opcode << " " << instruction_name << " ";

    if (instruction_bytes > 1) {
        for (uint8_t i = 0; i < instruction_bytes - 1; i++) {   // print as many operands as needed
            _current_address++;
            _bus_ptr->set_address(_current_address);
            operand = _bus_ptr->read_data();
            decoded_line << "0x" << std::setfill('0') << std::setw(2) << std::uppercase << std::hex << operand << " ";
        }
    }

    decoded_line << "[" << address_mode << "]"; 
    _decoded_instruction = decoded_line.str();
    _current_address++;
}

