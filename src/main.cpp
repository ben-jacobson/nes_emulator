#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include <iostream>
#include <string>

#include "memory_map.h"
#include "helpers.h"
#include "status_graphics.h"
#include "processor_status_graphics.h"
#include "memory_status_graphics.h"
#include "memory_peek_graphics.h"
#include "instr_trace_graphics.h"
#include "instruction_log.h"

#include "bus.h"
#include "cpu.h"
#include "ram.h"
#include "cartridge.h"

int main(int argc, char *argv[])
{
	constexpr uint16_t SCREEN_WIDTH = 1280;
	constexpr uint16_t SCREEN_HEIGHT = 720;
	// constexpr uint8_t TARGET_FRAMERATE = 60;  60 fps
	// constexpr uint16_t SCREEN_TICKS_PER_FRAME = 1000 / TARGET_FRAMERATE;

	// Create a base path for loading in assets
	char *base_path = SDL_GetBasePath();
	
    if (!base_path) {
        base_path = SDL_strdup("./");
    }   

	// initialize our bus, ram and cpu
	bus nes_bus;
	ram nes_ram(&nes_bus, RAM_SIZE_BYTES, RAM_ADDRESS_SPACE_START, RAM_ADDRESS_SPACE_END);
	cartridge nes_cart(&nes_bus, CART_ADDRESS_SPACE_START, CART_ADDRESS_SPACE_END);
	cpu nes_cpu(&nes_bus);  

	// register the devices that live on the bus
	nes_bus.register_new_bus_device(RAM_ADDRESS_SPACE_START, RAM_ADDRESS_SPACE_END, nes_ram._read_function_ptr, nes_ram._write_function_ptr);
	nes_bus.register_new_bus_device(CART_ADDRESS_SPACE_START, CART_ADDRESS_SPACE_END, nes_cart._read_function_ptr);	

	// Check to see if we can load a ROM
	if (argc < 2) {
		std::cout << "No ROM file specified in argument, loading default test code" << std::endl;
		nes_cart.load_content_from_stream("E8 E8 EA CA 58 78"); // INX, INX, NOP, DEX, CLI, SEI
	}
	else {
		std::string rom_fullpath = (std::string)base_path;
		rom_fullpath.append(argv[1]); 
		
		std::cout << "reading: " << rom_fullpath << std::endl;

		if (!nes_cart.load_rom(rom_fullpath)) {
			std::cout << "Could not load ROM file: " << rom_fullpath << ", errno: " << errno << std::endl;
			return 0;
		}
		else {
			std::cout << "Loaded " << rom_fullpath << std::endl;

			// temporarily set the reset vector, so that CPU starts at 0xC000 to run the tests; at the moment the test rom takes it to another area presumably to render some graphics which we couldn't handle at the moment
			nes_cart.load_content_from_stream("00 C0", RESET_VECTOR_LOW); // DELETE THIS LINE!
		}
	}
	
	// Init SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {        
		std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
		return 0;
    }
	
    // On success, setup SDL window and rendered
	SDL_Window* window = SDL_CreateWindow("6502 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // black, full alpha

	// reset the cpu before kicking off the loop. this must be done before the main loop, but must not be done before registering devices
	nes_cpu.reset();
	bool run_mode = false; // flag for whether or not code will run automatically, set to false since we want to manually step through instructions for a while. 
	bool single_cycle = true; // set to true initially so as to cycle through the reset cycles
	std::cout << "Emulator started. Press <Space> to step through instructions, <F5> to toggle run, <Del> to reset and <ESC> to quit" << std::endl;

	// Some font objects
	uint8_t font_size = 14; 
	std::string font_fullpath = (std::string)base_path;
	font_fullpath.append("C64_Pro_Mono-STYLE.ttf"); 

	// set up our instruction trace logger
	std::string log_fullpath = (std::string)base_path;
	log_fullpath.append("emulator_trace.log"); 
	instruction_log instruction_trace_log(log_fullpath, &nes_cpu, &nes_bus);

	// set up our debug display objects, 
	game_display_placeholder_output placeholder_game_area_rect(renderer, 20, 20, 2);
	instr_trace_graphics debug_instr_trace(&instruction_trace_log, renderer, font_fullpath.c_str(), font_size, 0, 520);
	processor_status_graphics debug_processor_status(&nes_cpu, renderer, font_fullpath.c_str(), font_size, 20 + 512 + 20, 520);
	memory_peek_graphics debug_memory_peek(&nes_bus, renderer, font_fullpath.c_str(), font_size, 20 + 512 + 20, 520 + (font_size * 8)); // 7 lines below processor status
	memory_status_graphics debug_ram_display(&nes_bus, renderer, font_fullpath.c_str(), font_size, 20 + 512 + 20, 20, "RAM Contents", RAM_ADDRESS_SPACE_START);
	memory_status_graphics debug_rom_display(&nes_bus, renderer, font_fullpath.c_str(), font_size, 20 + 512 + 20, 25 + (18 * font_size), "ROM Contents", nes_cpu.get_program_counter()); 

	// SDL event handler, including a keyboard event
	SDL_Event event_handler; 
	SDL_KeyboardEvent *key_event;
	memory_peek_text_input_processor memory_peek_text_input;
	SDL_StopTextInput();	// stop text input by default

	bool quit = false; 

	//bool nes_tests_error_code_found = false;
	//uint16_t nop_tracker = 0x0000; // nes tests uses nops to indicate the start of the test. we'll continually update the 
	uint16_t halt_at_pc = 0xD936;

	while (!quit) { // main application running loop

		if (nes_cpu.finished_instruction()) { 		// only update the screen if the instruction has finished, saving us many re-displays
			// update the log
			instruction_trace_log.update();

			// clear the screen
			SDL_RenderClear(renderer); 
			
			//draw the game area placeholder
			placeholder_game_area_rect.draw();	

			// draw the debug emulator status displays
			debug_instr_trace.display_contents();	
			debug_processor_status.display_contents();
			debug_ram_display.display_contents();
			debug_rom_display.display_contents(); 	
			debug_memory_peek.display_contents();

			// update the display with new info from renderer
			SDL_RenderPresent(renderer);	

			// Cap to roughly 60 FPS, we'll work out something a bit more official shortly. 
			//SDL_Delay(16); 
		}

		// process the CPU as needed by the user
		if (run_mode || single_cycle) {
			nes_cpu.cycle();

			if (!run_mode && nes_cpu.finished_instruction()) { // run the cpu until the instruction finishes
				//std::cout << "CPU cycle: " << nes_cpu.debug_get_cycle_count() << std::endl;
				single_cycle = false;
			}

			/*if (nes_cpu.get_last_fetched_opcode() == 0xEA) {
				nop_tracker = nes_cpu.get_program_counter() - 1;	// keep track of the last unit being tested
			}*/

			/*if (run_mode && nes_tests_error_code_found == false) && (nes_ram.read(0x0000) != 0x00 || nes_ram.read(0x0003) != 0x00)) { // indicating that the test rom has found an error
				std::cout << "Halting at nestest error detection. Start of test: " << "0x" << std::uppercase << std::hex << nop_tracker << std::dec << std::endl;
				halt_at_pc = nop_tracker; 	// set this up to halt at the start of the last test
				nes_tests_error_code_found = true;		
			}*/		

			if (run_mode && (/*nes_tests_error_code_found == true ||*/ nes_cpu._hit_break == true || nes_cpu.get_program_counter() == halt_at_pc)) {  // alternaitvely, nes_cpu.get_program_counter() == halt_at_pc || 
				std::cout << "Halting at 0x" << std::hex << std::uppercase << nes_cpu.get_program_counter() << std::dec << std::endl;
				run_mode = false;
				single_cycle = true; // get this up to the next cycle
				//nes_tests_error_code_found = false; // allows you to continue execution if you choose. 
			}	
		}

		// For gameplay keypresses, we don't want any delay on the keys, so we handle them with a keyboard state, outside of the event handler
		const Uint8* keystates = SDL_GetKeyboardState(NULL);

		if (keystates[SDL_SCANCODE_UP]) {
			std::cout << "Up Key" << std::endl;
		}
		else if (keystates[SDL_SCANCODE_DOWN]) { // being a DPad, it's not possible to press up and down at the same time, either one or the other will be pressed
			std::cout << "Down Key" << std::endl;
		}

		if (keystates[SDL_SCANCODE_LEFT]) {
			std::cout << "Left Key" << std::endl;
		}
		else if (keystates[SDL_SCANCODE_RIGHT]) { // being a DPad, it's not possible to press left and right at the same time, either one or the other will be pressed
			std::cout << "Right Key" << std::endl;
		}
		
		// Handle all events on queue, including the call for quit
		while (SDL_PollEvent(&event_handler) != 0) {		
			if (event_handler.type == SDL_KEYUP) {
				key_event = &event_handler.key;

				switch (key_event->keysym.sym) {
					case SDLK_TAB:	// Enter memory peek mode
						SDL_StartTextInput();
						debug_memory_peek.activate_cursor();
						break;

					case SDLK_RETURN:	// Finish memory peek mode
						if (SDL_IsTextInputActive()) {
							debug_memory_peek.set_address(memory_peek_text_input.process());		// if user presses less than 4 chars, pad with zeros and go with what was entered						
							SDL_StopTextInput();
						}
						break;

					case SDLK_F5: // Toggle run mode
						std::cout << "Run mode: " << (run_mode ? "off" : "on") << std::endl;
						run_mode = !run_mode; 
						break;

					case SDLK_SPACE:	// Single cycle through CPU
						if (!run_mode)  // cycle the cpu, but only if not in run mode, we don't want to cycle twice in one main loop.
						single_cycle = true;
						break;

					case SDLK_DELETE:		// RESET CPU and RAM
						nes_ram.clear_ram();
						nes_cpu.reset();
						single_cycle = true; // do this so that the processor can progress the first initial clock cycles and pause on the first instruction
						std::cout << "CPU Reset" << std::endl;
						break;

					case SDLK_ESCAPE:
						quit = true;	// quit the program
						break;																	
				}
			}
			else if (event_handler.type == SDL_TEXTINPUT) {
				char key_pressed = event_handler.text.text[0];	

				debug_memory_peek.input_partial_address(key_pressed);

				if (memory_peek_text_input.add_character(key_pressed)) {	// returns true if 4 characters have been entered, so that user doesn't have to press enter, they can just pop in their 4 chars
					debug_memory_peek.set_address(memory_peek_text_input.process());							
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