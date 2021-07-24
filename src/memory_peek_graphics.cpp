#include "memory_peek_graphics.h"

memory_peek_graphics::memory_peek_graphics(bus* bus_ptr, SDL_Renderer* renderer, const char* font_filename, int ptsize, uint16_t preset_display_x, uint16_t preset_display_y)
    : status_graphics(renderer, font_filename, ptsize)
{
    _bus_ptr = bus_ptr;
    _address = 0;

    _preset_display_x = preset_display_x;
    _preset_display_y = preset_display_y;
}

void memory_peek_graphics::display_contents(void) {
    std::stringstream peek_contents_line;
    _bus_ptr->set_address(_address);
    peek_contents_line << "Peek address: "; 
    peek_contents_line << "0x" << std::setfill('0') << std::setw(4) << std::uppercase << std::hex << _address << ": ";            // first line is the 0 memory address     
    peek_contents_line << "0x" << std::setfill('0') << std::setw(2) << std::uppercase << std::hex << (uint16_t)_bus_ptr->read_data();
    draw_to_buffer(peek_contents_line.str(), _preset_display_x, _preset_display_y);
}

void memory_peek_graphics::set_address(uint16_t address) {
    _address = address;
}