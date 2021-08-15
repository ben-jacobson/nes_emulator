#include "memory_peek_graphics.h"

memory_peek_graphics::memory_peek_graphics(bus* bus_ptr, SDL_Renderer* renderer, const char* font_filename, int ptsize, std::string headline, uint16_t preset_display_x, uint16_t preset_display_y)
    : status_graphics(renderer, font_filename, ptsize)
{
    _bus_ptr = bus_ptr;
    _address = 0;

    _preset_display_x = preset_display_x;
    _preset_display_y = preset_display_y;

    _cursor_active = false; 
    _headline = headline; 
}

void memory_peek_graphics::display_contents(void) {
    std::stringstream peek_contents_line;
    uint16_t peek_contents_line_len = 0;

    _bus_ptr->set_address(_address);

    peek_contents_line << _headline << ": "; 
    set_colour({255, 255, 255, 255});
    draw_to_buffer(peek_contents_line.str(), _preset_display_x, _preset_display_y);
    
    // work out the length, then reset the string contents
    peek_contents_line_len = peek_contents_line.str().length();
    peek_contents_line = std::stringstream();

    if (_cursor_active) {
        // change the colour of the text to grey and draw the partial address
        set_colour({128, 128, 128, 255});
        peek_contents_line << "0x" << std::setfill('0') << std::setw(4) << std::uppercase << std::hex << _partial_address << ": ";            // first line is the 0 memory address     
    }
    else {
        set_colour({255, 255, 255, 255});
        peek_contents_line << "0x" << std::setfill('0') << std::setw(4) << std::uppercase << std::hex << _address << ": ";            // first line is the 0 memory address     
    }    

    // draw the address to screen
    draw_to_buffer(peek_contents_line.str(), _preset_display_x + (peek_contents_line_len * _font_width), _preset_display_y);

    // work out the length, then reset the string contents
    peek_contents_line_len += peek_contents_line.str().length();
    peek_contents_line = std::stringstream();

    peek_contents_line << "0x" << std::setfill('0') << std::setw(2) << std::uppercase << std::hex << (uint16_t)_bus_ptr->read_data();

    set_colour({255, 255, 255, 255});
    draw_to_buffer(peek_contents_line.str(), _preset_display_x + (peek_contents_line_len * _font_width), _preset_display_y);
}

void memory_peek_graphics::activate_cursor(void) {
    _cursor_active = true;
}

void memory_peek_graphics::input_partial_address(char input) {
    _partial_address += toupper(input);
}

void memory_peek_graphics::set_address(uint16_t address) {
    _address = address;

    _cursor_active = false;
    _partial_address = std::string();
}