#pragma once

#include <SDL2/SDL.h>
#include <array>

#include "text_renderer.h"

constexpr uint8_t MAX_FRAMES_TO_AVERAGE = 32;

class fps_counter
{
public:
	fps_counter(text_renderer* text_surface, uint16_t preset_display_x, uint16_t preset_display_y);
	~fps_counter() = default;

	void get_frame_start_time(void);
	uint16_t debug_report_last_frame_time(void);
	void display_contents(void);

private:
	text_renderer* _text_surface;
    uint16_t _preset_display_x, _preset_display_y; 	
	uint32_t _frame_render_start;

	// for averaging out our frames
	std::array<uint16_t, MAX_FRAMES_TO_AVERAGE> _average_frame_time_array;
	uint16_t _average_frame_time; 
	uint8_t _frame_count;

	void recalculate_average_frame_time(void);
};
