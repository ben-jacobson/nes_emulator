#include "memory_status_graphics.h"

memory_status_graphics::memory_status_graphics(uint16_t start_address, SDL_Renderer* renderer, const char* font_filename, int ptsize, bus* bus_ptr)
    : status_graphics(renderer, font_filename, ptsize), _START_ADDRESS(start_address)
{
    _bus_ptr = bus_ptr;
}

memory_status_graphics::memory_status_graphics(uint16_t start_address, SDL_Renderer* renderer, uint16_t preset_display_x, uint16_t preset_display_y, const char* font_filename, int ptsize, bus* bus_ptr)
    : memory_status_graphics(start_address, renderer, font_filename, ptsize, bus_ptr)
{
    _preset_display_x = preset_display_x;
    _preset_display_y = preset_display_y;
}  


void memory_status_graphics::display_contents(uint16_t x, uint16_t y) {
    uint16_t rows = 0; 
    std::stringstream memory_contents_line;
    memory_contents_line << "0x" << std::setfill('0') << std::setw(4) << std::uppercase << std::hex << _START_ADDRESS << ": ";            // first line is the 0 memory address     

    for (uint16_t i = 0; i < BYTES_TO_DISPLAY; i++) {
        if (i > 0) {
            if (i % (8 * 8) == 0) {
                rows++; // add an extra row to rendering every 8 x 8 matrix
            }

            if (i % 8 == 0) {   // at end of every 8 columns create a new line
                draw_to_buffer(memory_contents_line.str(), x, y + (rows * _font_height));
                memory_contents_line = std::stringstream(); // string streams don't have a working clear method, so this will have to do 
                memory_contents_line << "0x" << std::setfill('0') << std::setw(4) << std::uppercase << std::hex << (uint16_t)i << ": ";                
                rows++;
            }                
        }
        _bus_ptr->set_address(_START_ADDRESS + i);
        memory_contents_line << std::setfill('0') << std::setw(2) << std::uppercase << std::hex << (uint16_t)_bus_ptr->read_data() << " ";
    }
}

void memory_status_graphics::display_contents(void) {
    display_contents(_preset_display_x, _preset_display_y);
}