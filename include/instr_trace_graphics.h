#pragma once

#include <SDL2/SDL_ttf.h>
#include <iomanip>

#include "status_graphics.h"
#include "bus.h"
#include "cpu.h"

class instr_trace_graphics : public status_graphics
{
public:
	instr_trace_graphics(cpu* cpu_ptr, bus* bus_ptr, SDL_Renderer* renderer, const char* font_filename, int ptsize, uint16_t preset_display_x, uint16_t preset_display_y);
	~instr_trace_graphics() = default;

    void display_contents(void);

private:
    static constexpr uint8_t INSTRUCTIONS_TO_DISPLAY = 12;
    uint16_t _current_address;
    uint8_t last_instruction_width;
    std::string _decoded_instruction;

    uint16_t _preset_display_x, _preset_display_y; 

    bus* _bus_ptr;
    cpu* _cpu_ptr; // for viewing the instruction opcodes + address modes data

    void fetch_and_decode_next_instruction(void);
};
