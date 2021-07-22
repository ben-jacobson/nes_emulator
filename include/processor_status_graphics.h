#pragma once

#include "status_graphics.h"
#include <iomanip>

#include "cpu.h"

class processor_status_graphics : public status_graphics
{
public:
	processor_status_graphics(cpu* cpu_ptr, SDL_Renderer* renderer, const char* font_filename, int ptsize, uint16_t preset_display_x, uint16_t preset_display_y);
	~processor_status_graphics() = default;

	void display_contents(void);

private:
    uint16_t _preset_display_x, _preset_display_y; 
    cpu* _cpu_ptr; // for viewing the status
};
