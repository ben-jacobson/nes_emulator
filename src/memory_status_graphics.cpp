#include "memory_status_graphics.h"

ram_status_output::ram_status_output(SDL_Renderer* renderer, const char* font_filename, int ptsize, ram* ram_ptr)
    : emulator_status_graphics(renderer, font_filename, ptsize)
{
    _ram_ptr = ram_ptr;
}

void ram_status_output::display_ram_contents(uint16_t x, uint16_t y) {
    uint16_t rows = 0; 
    std::stringstream memory_contents_line;
    memory_contents_line << "0x" << std::setfill('0') << std::setw(2) << std::uppercase << std::hex << 0 << ": ";            // first line is the 0 memory address     

    for (uint16_t i = 0; i < BYTES_TO_DISPLAY; i++) {
        if (i > 0) {
            if (i % (8 * 8) == 0) {
                rows++; // add an extra row to rendering every 8 x 8 matrix
            }

            if (i % 8 == 0) {   // at end of every 8 columns create a new line
                draw_to_buffer(memory_contents_line.str(), x, y + (rows * _font_height));
                memory_contents_line = std::stringstream(); // string streams don't have a working clear method, so this will have to do 
                memory_contents_line << "0x" << std::setfill('0') << std::setw(2) << std::uppercase << std::hex << (int)i << ": ";                
                rows++;
            }                
        }
        memory_contents_line << std::setfill('0') << std::setw(2) << std::uppercase << std::hex << (int)_ram_ptr->debug_read(i) << " ";
    }
}