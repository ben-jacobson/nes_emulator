#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include <iostream>
#include <string>

#include "memory_map.h"
#include "status_graphics.h"
#include "memory_status_graphics.h"
#include "instr_trace_graphics.h"

#include "bus.h"
#include "cpu.h"
#include "ram.h"
#include "cartridge.h"

class game_display_placeholder_output {
public:

	game_display_placeholder_output(SDL_Renderer* renderer, uint16_t x_pos, uint16_t y_pos, uint8_t scale) {
		_renderer = renderer;

		_rect.x = x_pos;
		_rect.y = y_pos;
		_rect.w = RECT_WIDTH * scale; 
		_rect.h = RECT_HEIGHT * scale;		

		// create a placeholder rectangle for when we eventually want to render some display
		_surface = SDL_CreateRGBSurface(0, _rect.w, _rect.h, 32, 0, 0, 0, 0); // 32 bit pixel depth

		if (_surface == NULL) {
			std::cout << "CreateRGBSurface failed: " << SDL_GetError() << std::endl;
			exit(1);
		}

		SDL_FillRect(_surface, NULL, SDL_MapRGB(_surface->format, 64, 64, 64));
		_texture = SDL_CreateTextureFromSurface(_renderer, _surface);
	}

	~game_display_placeholder_output() {
		SDL_FreeSurface(_surface);
		SDL_DestroyTexture(_texture);
	}

	void draw(void) {
		SDL_RenderCopy(_renderer, _texture, NULL, &_rect);
	}

private:
	static constexpr uint16_t RECT_WIDTH = 256, RECT_HEIGHT = 240;	// matches resolution of the original nes. 

	uint16_t _xpos, _ypos; 
	SDL_Renderer* _renderer;
	SDL_Surface* _surface; 
	SDL_Rect _rect;
	SDL_Texture* _texture;
};

int main()
{
	constexpr uint16_t SCREEN_WIDTH = 1280;
	constexpr uint16_t SCREEN_HEIGHT = 720;
	// constexpr uint8_t TARGET_FRAMERATE = 60;  60 fps
	// constexpr uint16_t SCREEN_TICKS_PER_FRAME = 1000 / TARGET_FRAMERATE;

	// initialize our bus, ram and cpu
	bus nes_bus;
	ram nes_ram(&nes_bus, RAM_SIZE_BYTES, RAM_ADDRESS_SPACE_START, RAM_ADDRESS_SPACE_END);
	cartridge nes_cart(&nes_bus, CART_ADDRESS_SPACE_START, CART_ADDRESS_SPACE_END);
	cpu nes_cpu(&nes_bus);  // todo, add the PPU

	nes_bus.register_new_bus_device(RAM_ADDRESS_SPACE_START, RAM_ADDRESS_SPACE_END, nes_ram._read_function_ptr, nes_ram._write_function_ptr);
	nes_bus.register_new_bus_device(CART_ADDRESS_SPACE_START, CART_ADDRESS_SPACE_END, nes_cart._read_function_ptr);	

	// Attempt to init SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {        
		std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
		return 0;
    }

	// start by creating a base path
	char *base_path = SDL_GetBasePath();
	//size_t base_path_len = strlen(base_path);
	
    if (!base_path) {
        base_path = SDL_strdup("./");
    }   // std::cout << "Base path set at: " << test << std::endl;
	
    // On success, setup SDL window and rendered
	SDL_Window* window = SDL_CreateWindow("6502 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // black, full alpha


	// initialize our font object
    if(!TTF_WasInit() && TTF_Init() != 0) {
        std::cout << "TTF_Init unsuccessful: " << TTF_GetError() << std::endl;
        exit(EXIT_FAILURE);
	}

	uint8_t font_size = 14; 
	std::string font_fullpath = ((std::string)base_path).append("C64_Pro_Mono-STYLE.ttf").c_str();
	status_graphics test_message(renderer, font_fullpath.c_str(), font_size);  
	test_message.set_colour({128, 128, 128, 255});

	// set up our display objects, 
	game_display_placeholder_output placeholder_game_area_rect(renderer, 20, 20, 2);
	instr_trace_graphics debug_instr_trace(&nes_bus, ROM_ADDRESS_SPACE_START, renderer, font_fullpath.c_str(), font_size, 20, 520);
	memory_status_graphics debug_ram_display("RAM Contents", RAM_ADDRESS_SPACE_START, renderer, 560, 20, font_fullpath.c_str(), font_size, &nes_bus);
	memory_status_graphics debug_rom_display("ROM Contents", ROM_ADDRESS_SPACE_START, renderer, 560, 25 + (18 * font_size), font_fullpath.c_str(), font_size, &nes_bus);

	//bus* bus_ptr, uint16_t start_address, SDL_Renderer* renderer, const char* font_filename, int ptsize, uint16_t preset_display_x, uint16_t preset_display_y

	SDL_Event event_handler; 
	bool quit = false; 

	uint16_t x_pos = 100, y_pos = 100; 
	int8_t x_speed = 2, y_speed = 2; 
    std::string text_to_render; 

	while (!quit) { // main application running loop
		// clear the screen
		SDL_RenderClear(renderer); 
		
		//draw the game area placeholder
		placeholder_game_area_rect.draw();	

		// draw the debug emulator status items
		debug_ram_display.display_contents();
		debug_rom_display.display_contents(); 	
		debug_instr_trace.display_contents();	

		// draw the moving text, which helps us see if we are rendering at an acceptable FPS
		text_to_render = "x: " + std::to_string(x_pos) + ", y: " + std::to_string(y_pos);
		test_message.draw_to_buffer(text_to_render, x_pos, y_pos);	

		// update the display with new info from renderer
		SDL_RenderPresent(renderer);	

		// update all of our text posiitons, 
		x_pos += x_speed;  //controls the rect's x coordinate 
		y_pos += y_speed;  // controls the rect's y coordinate		

		if (x_pos <= 0 || x_pos + test_message.get_text_width(text_to_render) >= SCREEN_WIDTH) {
			x_speed = -x_speed;
		}

		if (y_pos <= 0 || y_pos + test_message.get_text_height(text_to_render) >= SCREEN_HEIGHT) {
			y_speed = -y_speed;
		}	

		// Cap to roughly 60 FPS, we'll work out something a bit more official shortly. 
		SDL_Delay(16); 

		// Handle all events on queue
		while (SDL_PollEvent(&event_handler) != 0) {		
			if (event_handler.type == SDL_QUIT) {	
				quit = true;
			}
		}		
	}

	// tidy up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}