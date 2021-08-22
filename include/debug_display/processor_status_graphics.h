#pragma once

#include "text_renderer.h"
#include <iomanip>

#include "cpu.h"
#include "ppu.h"

class processor_status_graphics
{
public:
	processor_status_graphics(text_renderer* text_surface, cpu* cpu_ptr, ppu* ppu_ptr, uint16_t preset_display_x, uint16_t preset_display_y);
	~processor_status_graphics() = default;

	void display_contents(void);

private:
	text_renderer* _text_surface;
	
    uint16_t _preset_display_x, _preset_display_y; 
    cpu* _cpu_ptr; // for viewing the status
	ppu* _ppu_ptr;
};
