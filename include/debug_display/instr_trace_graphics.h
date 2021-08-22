#pragma once

#include <SDL2/SDL_ttf.h>
#include <iomanip>

#include "instruction_log.h"
#include "text_renderer.h"
#include "bus.h"
#include "cpu.h"

class instr_trace_graphics
{
public:
	instr_trace_graphics(text_renderer* text_surface, instruction_log* log_ptr, uint16_t preset_display_x, uint16_t preset_display_y);
	~instr_trace_graphics() = default;

    void display_contents(void);

private:
	text_renderer* _text_surface;

    uint16_t _preset_display_x, _preset_display_y; 
    instruction_log* _log_ptr;
};
