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
	static constexpr uint16_t RECT_WIDTH = 256, RECT_HEIGHT = 240;	// matches resolution of the original nes. 

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
	cpu nes_cpu(&nes_bus);  

	// register the devices that live on the bus
	nes_bus.register_new_bus_device(RAM_ADDRESS_SPACE_START, RAM_ADDRESS_SPACE_END, nes_ram._read_function_ptr, nes_ram._write_function_ptr);
	nes_bus.register_new_bus_device(CART_ADDRESS_SPACE_START, CART_ADDRESS_SPACE_END, nes_cart._read_function_ptr);	

	// Load some content into the ROM
	nes_cart.load_content_from_stream("E8 E8 EA CA"); // INX, INX, NOP, DEX

	// Init SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {        
		std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
		return 0;
    }
	
	// Create a base path for loading in assets
	char *base_path = SDL_GetBasePath();
	
    if (!base_path) {
        base_path = SDL_strdup("./");
    }   
	
    // On success, setup SDL window and rendered
	SDL_Window* window = SDL_CreateWindow("6502 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // black, full alpha

	// Some font objects
	uint8_t font_size = 14; 
	std::string font_fullpath = (std::string)base_path;
	font_fullpath.append("C64_Pro_Mono-STYLE.ttf"); 

	// set up our debug display objects, 
	game_display_placeholder_output placeholder_game_area_rect(renderer, 20, 20, 2);
	instr_trace_graphics debug_instr_trace(&nes_cpu, &nes_bus, ROM_ADDRESS_SPACE_START, renderer, font_fullpath.c_str(), font_size, 0, 520);
	memory_status_graphics debug_ram_display("RAM Contents", RAM_ADDRESS_SPACE_START, renderer, 20 + 512 + 20, 20, font_fullpath.c_str(), font_size, &nes_bus);
	memory_status_graphics debug_rom_display("ROM Contents", ROM_ADDRESS_SPACE_START, renderer, 20 + 512 + 20, 25 + (18 * font_size), font_fullpath.c_str(), font_size, &nes_bus);

	// SDL event handler, including a keyboard event
	SDL_Event event_handler; 
	SDL_KeyboardEvent *key_event;
	bool quit = false; 

	// reset the cpu before kicking off the loop. this must be done before the main loop, but must not be done before registering devices
	nes_cpu.reset();
	bool run_mode = false; // flag for whether or not code will run automatically, set to false since we want to manually step through instructions for a while. 

	while (!quit) { // main application running loop
		// clear the screen
		SDL_RenderClear(renderer); 
		
		//draw the game area placeholder
		placeholder_game_area_rect.draw();	

		// draw the debug emulator status displays
		debug_instr_trace.display_contents();	
		debug_ram_display.display_contents();
		debug_rom_display.display_contents(); 	

		// update the display with new info from renderer
		SDL_RenderPresent(renderer);	

		// Cap to roughly 60 FPS, we'll work out something a bit more official shortly. 
		SDL_Delay(16); 

		// For gameplay keypresses, we don't want any delay on the keys, so we handle them slightly differently
		const Uint8* keystates = SDL_GetKeyboardState(NULL);

		if (keystates[SDL_SCANCODE_UP]) {
			std::cout << "Up Key" << std::endl;
		}
		if (keystates[SDL_SCANCODE_DOWN]) {
			std::cout << "Down Key" << std::endl;
		}
		if (keystates[SDL_SCANCODE_LEFT]) {
			std::cout << "Left Key" << std::endl;
		}
		if (keystates[SDL_SCANCODE_RIGHT]) {
			std::cout << "Right Key" << std::endl;
		}

		if (run_mode) {
			// nes_cpu.cycle(); 
		}

		// Handle all events on queue, including the call for quit
		while (SDL_PollEvent(&event_handler) != 0) {		
			if (event_handler.type == SDL_KEYUP) {
				key_event = &event_handler.key;

				switch (key_event->keysym.sym) {
					case SDLK_F5:
						// toggle run mode
						run_mode = !run_mode; 
						break;

					case SDLK_SPACE:
						if (!run_mode)  // cycle the cpu, but only if not in run mode, we don't want to cycle twice in one main loop.
							nes_cpu.cycle();
						break;

					case SDLK_DELETE:
						nes_cpu.reset();
						break;

					case SDLK_TAB:
						// allow editing for a memory region
						break;

					case SDLK_ESCAPE:
						quit = true;	// quit the program
						break;																	
				}
			}

			else if (event_handler.type == SDL_QUIT) {
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