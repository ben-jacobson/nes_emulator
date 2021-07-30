#pragma once

#include <SDL2/SDL_ttf.h>
#include <iomanip>

#include "instruction_log.h"
#include "status_graphics.h"
#include "bus.h"
#include "cpu.h"

class instr_trace_graphics : public status_graphics
{
public:
	instr_trace_graphics(instruction_log* log_ptr, SDL_Renderer* renderer, const char* font_filename, int ptsize, uint16_t preset_display_x, uint16_t preset_display_y);
	~instr_trace_graphics() = default;

    void display_contents(void);

private:
    uint16_t _preset_display_x, _preset_display_y; 
    instruction_log* _log_ptr;
};
