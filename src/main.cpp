#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include <iostream>
#include <string>

#include "memory_map.h"
#include "helpers.h"
#include "status_graphics.h"
#include "processor_status_graphics.h"
#include "memory_status_graphics.h"
#include "instr_trace_graphics.h"

#include "bus.h"
#include "cpu.h"
#include "ram.h"
#include "cartridge.h"

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
	nes_cart.load_content_from_stream("E8 E8 EA CA 58 78"); // INX, INX, NOP, DEX, CLI, SEI

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
	instr_trace_graphics debug_instr_trace(&nes_cpu, &nes_bus, renderer, font_fullpath.c_str(), font_size, 0, 520);
	processor_status_graphics debug_processor_status(&nes_cpu, renderer, font_fullpath.c_str(), font_size, 20 + 512 + 20, 520);
	memory_status_graphics debug_ram_display(&nes_bus, renderer, font_fullpath.c_str(), font_size, 20 + 512 + 20, 20, "RAM Contents", RAM_ADDRESS_SPACE_START);
	memory_status_graphics debug_rom_display(&nes_bus, renderer, font_fullpath.c_str(), font_size, 20 + 512 + 20, 25 + (18 * font_size), "ROM Contents", ROM_ADDRESS_SPACE_START);

	// SDL event handler, including a keyboard event
	SDL_Event event_handler; 
	SDL_KeyboardEvent *key_event;
	std::string address_peek_text_input; 
	uint8_t address_peek_chars_inputted = 0; // when we've entered 4 chars, it will auto-complete
	SDL_StopTextInput();

	// reset the cpu before kicking off the loop. this must be done before the main loop, but must not be done before registering devices
	nes_cpu.reset();
	bool run_mode = false; // flag for whether or not code will run automatically, set to false since we want to manually step through instructions for a while. 
	bool single_cycle = true; // set to true initially so as to cycle through the reset cycles

	std::cout << "Emulator started. Press <Space> to step through instructions, <F5> to toggle run, <Del> to reset and <ESC> to quit" << std::endl;

	bool quit = false; 

	while (!quit) { // main application running loop
		// clear the screen
		SDL_RenderClear(renderer); 
		
		//draw the game area placeholder
		placeholder_game_area_rect.draw();	

		// draw the debug emulator status displays
		debug_instr_trace.display_contents();	
		debug_processor_status.display_contents();
		debug_ram_display.display_contents();
		debug_rom_display.display_contents(); 	

		// update the display with new info from renderer
		SDL_RenderPresent(renderer);	

		if (run_mode) {
			nes_cpu.cycle(); 
		}

		if (single_cycle) {
			nes_cpu.cycle();

			if (nes_cpu.finished_instruction()) { // run the cpu until the instruction finishes
				std::cout << "CPU cycle: " << nes_cpu.debug_get_cycle_count() << std::endl;
				single_cycle = false;
			}
		}

		// Cap to roughly 60 FPS, we'll work out something a bit more official shortly. 
		//SDL_Delay(16); 

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
		
		// Handle all events on queue, including the call for quit
		while (SDL_PollEvent(&event_handler) != 0) {		
			if (event_handler.type == SDL_KEYUP) {
				key_event = &event_handler.key;

				switch (key_event->keysym.sym) {
					case SDLK_TAB:
						address_peek_text_input = std::string(); // clear it
						SDL_StartTextInput();
						break;
					case SDLK_RETURN:
						if (SDL_IsTextInputActive()) {
							std::string padded_text_input = std::string(4 - address_peek_text_input.length(), '0') + address_peek_text_input;
							std::cout << "User entered: " << padded_text_input << std::endl; // todo, put this into own function
							address_peek_text_input = std::string(); // clear it
							address_peek_chars_inputted = 0;
							SDL_StopTextInput();
						}
						break;

					case SDLK_F5:
						// toggle run mode
						std::cout << "Run mode: " << (run_mode ? "off" : "on") << std::endl;
						run_mode = !run_mode; 
						break;

					case SDLK_SPACE:
						if (!run_mode)  // cycle the cpu, but only if not in run mode, we don't want to cycle twice in one main loop.
						single_cycle = true;
						break;

					case SDLK_DELETE:
						nes_cpu.reset();
						single_cycle = true; // do this so that the processor can progress the first 6 clock cycles and pause on the first instruction
						std::cout << "CPU reset" << std::endl;
						break;

					case SDLK_ESCAPE:
						quit = true;	// quit the program
						break;																	
				}
			}
			else if (event_handler.type == SDL_TEXTINPUT) {		
				char key_pressed = toupper(event_handler.text.text[0]);
				if (hex_key(key_pressed)) {	 // only append if a hex key
					address_peek_text_input += key_pressed;
					std::cout << "Char: " << address_peek_text_input << std::endl;
					address_peek_chars_inputted++;
				}

				if (address_peek_chars_inputted >= 4) {
					std::string padded_text_input = std::string(4 - address_peek_text_input.length(), '0') + address_peek_text_input;
					std::cout << "User entered: " << padded_text_input << std::endl; // todo, put this into own function
					address_peek_text_input = std::string(); // clear it
					address_peek_chars_inputted = 0;
					SDL_StopTextInput();					
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