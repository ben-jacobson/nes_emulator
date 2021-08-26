#include "fps_counter.h"

fps_counter::fps_counter(text_renderer* text_surface, uint16_t preset_display_x, uint16_t preset_display_y) {
	_text_surface = text_surface;
    _preset_display_x = preset_display_x;
    _preset_display_y = preset_display_y; 	

    get_frame_start_time();
    _frame_count = 0;

    for (auto& a : _average_frame_time_array) a = 1;   // fill the array with 1 to start with. It's important these values never go to 0
}

void fps_counter::get_frame_start_time(void) {
	_frame_render_start = SDL_GetTicks();
}

uint16_t fps_counter::debug_report_last_frame_time(void) {
    return _average_frame_time_array[_frame_count - 1];
}


void fps_counter::recalculate_average_frame_time(void) {
    uint32_t sum = 0; // at the moment, each fps instance is 16bit number, and we have a maximum of 32 of these, this is large enough to store the largest possible sum and have plenty of room left over in case we want to take more averages later
    
    for (uint8_t i = 0; i < MAX_FRAMES_TO_AVERAGE; i++) {
        sum += _average_frame_time_array[i];
    }

    if (sum < 1) {
        sum = 1; // to avoid any divide by zero issues
    }
    _average_frame_time = sum / MAX_FRAMES_TO_AVERAGE;
}

void fps_counter::display_contents(void) {
    // add a new entry and recalculate the averages
    _average_frame_time_array[_frame_count] = SDL_GetTicks() - _frame_render_start; 
    recalculate_average_frame_time();

    // move where our next entry will go
    _frame_count++; // continually increment
    _frame_count %= MAX_FRAMES_TO_AVERAGE;  // ensure we don't exceed the array size
    
    // update the display
    std::string fps = std::to_string(1000 / _average_frame_time) + "fps";
    _text_surface->set_colour({64, 64, 64, 255});    // white
    _text_surface->draw_text(fps, _preset_display_x, _preset_display_y);    
}