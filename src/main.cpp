#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include <iostream>
#include <string>

#include "debug_display/helpers.h"
#include "debug_display/text_renderer.h"
#include "debug_display/processor_status_graphics.h"
#include "debug_display/memory_status_graphics.h"
#include "debug_display/memory_peek_graphics.h"
#include "debug_display/instr_trace_graphics.h"
#include "debug_display/instruction_log.h"
#include "debug_display/pattern_table_preview.h"
#include "debug_display/fps_counter.h"

#include "memory_map.h"
#include "bus.h"
#include "cpu.h"
#include "ppu.h"
#include "ppu_draw.h"
#include "ram.h"
#include "palette_ram.h"
#include "cartridge.h"
#include "apu_io.h"

int main(int argc, char *argv[])
{
	constexpr uint16_t SCREEN_WIDTH = 1280;
	constexpr uint16_t SCREEN_HEIGHT = 720;

	// Create a base path for loading in assets
	char *base_path = SDL_GetBasePath();
	
    if (!base_path) {
        base_path = SDL_strdup("./");
    }   

	// initialize our bus, ram and cpu
	bus nes_cpu_bus;
	ram nes_ram(RAM_SIZE_BYTES, RAM_ADDRESS_SPACE_START, RAM_ADDRESS_SPACE_END);
	cartridge nes_cart(CART_ADDRESS_SPACE_START, CART_ADDRESS_SPACE_END);
	apu_io nes_apu_io(IO_ADDRESS_SPACE_START, IO_ADDRESS_SPACE_END);
	cpu nes_cpu(&nes_cpu_bus);  
	
	// register the devices that live on the bus TODO - Remove the need for mapping addresses, just pass a bus_device pointer and have the function figure the rest out. 
	nes_cpu_bus.register_new_bus_device(RAM_ADDRESS_SPACE_START, RAM_ADDRESS_SPACE_END, &nes_ram);//, nes_ram._read_function_ptr, nes_ram._write_function_ptr);
	nes_cpu_bus.register_new_bus_device(CART_ADDRESS_SPACE_START, CART_ADDRESS_SPACE_END, &nes_cart);//, nes_cart._read_function_ptr);	
	nes_cpu_bus.register_new_bus_device(IO_ADDRESS_SPACE_START, IO_ADDRESS_SPACE_END, &nes_apu_io);//, nes_apu_io._read_function_ptr, nes_apu_io._write_function_ptr);

	// initialize our PPU bus, pattern tables and name tables
	bus nes_ppu_bus;
	ppu nes_ppu(&nes_cpu_bus, &nes_ppu_bus, &nes_cpu);
	palette_ram nes_palette_ram;

	// nametables, or VRAM
	ram nametable_0_memory(NAMETABLE_SIZE, NAMETABLE_0_START, NAMETABLE_0_END);
	ram nametable_1_memory(NAMETABLE_SIZE, NAMETABLE_1_START, NAMETABLE_1_END);
	ram nametable_2_memory(NAMETABLE_SIZE, NAMETABLE_2_START, NAMETABLE_2_END);
	ram nametable_3_memory(NAMETABLE_SIZE, NAMETABLE_3_START, NAMETABLE_3_END);

	nes_cpu_bus.register_new_bus_device(PPU_ADDRESS_SPACE_START, PPU_MIRROR_SPACE_END, &nes_ppu);//, nes_ppu._read_function_ptr, nes_ppu._write_function_ptr); // register the PPU to the CPU bus
	nes_ppu_bus.register_new_bus_device(CHR_ROM_START, CHR_ROM_END, &nes_cart, nes_cart._ppu_read_function_ptr, nes_cart._ppu_write_function_ptr); // this needs to point to special custom function pointers for PPU reads
	nes_ppu_bus.register_new_bus_device(PALETTE_RAM_INDEX_START, PALETTE_RAM_MIRROR_END, &nes_palette_ram);//, nes_palette_ram._read_function_ptr, nes_palette_ram._write_function_ptr);

	// register our nametables
	nes_ppu_bus.register_new_bus_device(NAMETABLE_0_START, NAMETABLE_0_END, &nametable_0_memory); //, nametable_0_memory._read_function_ptr, nametable_0_memory._write_function_ptr);
	nes_ppu_bus.register_new_bus_device(NAMETABLE_1_START, NAMETABLE_1_END, &nametable_1_memory); //, nametable_1_memory._read_function_ptr, nametable_1_memory._write_function_ptr);
	nes_ppu_bus.register_new_bus_device(NAMETABLE_2_START, NAMETABLE_2_END, &nametable_2_memory); //, nametable_2_memory._read_function_ptr, nametable_2_memory._write_function_ptr);
	nes_ppu_bus.register_new_bus_device(NAMETABLE_3_START, NAMETABLE_3_END, &nametable_3_memory); //, nametable_3_memory._read_function_ptr, nametable_3_memory._write_function_ptr);

	// we have a special case use case for the OAMDMA, this needs to be accessible by the CPU in one memory location alone
	nes_cpu_bus.register_new_bus_device(OAMDMA, OAMDMA, &nes_ppu); //, nes_ppu._read_function_ptr, nes_ppu._write_function_ptr);

	// Check to see if we can load a ROM from argc
	if (argc < 2) {
		std::cout << "No ROM file specified in argument, loading default test code" << std::endl;
	}
	else {
		std::string rom_fullpath = (std::string)base_path;
		rom_fullpath.append(argv[1]); 
		
		std::cout << "Reading: " << rom_fullpath << std::endl;

		if (!nes_cart.load_rom(rom_fullpath)) {
			std::cout << "Could not load ROM file: " << rom_fullpath << ", errno: " << errno << std::endl;
			return 0;
		}
		else {
			std::cout << "Successfully loaded " << rom_fullpath << std::endl;
		}
		
		//nes_cart.load_content_from_stream("00 C0", RESET_VECTOR_LOW); // while using nestests, we want to overwrite the reset vector to start the program elsewhere
		//std::cout << "Reset vector altered to be 0xC000 for debugging purposes" << std::endl;
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

	// reset the cpu and ppu before kicking off the loop. this must be done before the main loop, but must not be done before registering devices
	nes_cpu.reset();
	nes_ppu.reset();

	std::cout << "Emulation started. Keys:" << std::endl; 
	std::cout << "  <Space> Execute next instruction in single step mode" << std::endl;  
	std::cout << "  <F5> Toggle run, disabling single step mode" << std::endl;  
	std::cout << "  <Del> Reset CPU" << std::endl;  
	std::cout << "  <TAB> Peek at memory location on CPU bus" << std::endl;  
	std::cout << "  <F1> Peek at memory location on PPU bus" << std::endl;  
	std::cout << "  <P> Switch palette in pattern table preview window" << std::endl;  
	std::cout << "  <ESC> Quit" << std::endl;

	std::cout << "Emulator controls:" << std::endl; 
	std::cout << "  <UP/DOWN/LEFT/RIGHT> D-Pad controls" << std::endl;  
	std::cout << "  <Z> A" << std::endl;  
	std::cout << "  <X> B" << std::endl;  	
	std::cout << "  <N> Sel" << std::endl;  
	std::cout << "  <M> Start" << std::endl;  		

	// Some font objects
	uint8_t font_size = 14; 
	std::string font_fullpath = (std::string)base_path;
	font_fullpath.append("C64_Pro_Mono-STYLE.ttf"); 	

	// set up our instruction trace logger
	std::string log_fullpath = (std::string)base_path;
	log_fullpath.append("emulator_trace.log"); 
	instruction_log instruction_trace_log(log_fullpath, &nes_cpu, &nes_cpu_bus);

	// set up our debug display objects, 
	text_renderer text_surface(font_fullpath, font_size, renderer);
	game_display_placeholder_output placeholder_game_area_rect(renderer, 20, 20, 2);
	instr_trace_graphics debug_instr_trace(&text_surface, &instruction_trace_log, 0, 520);
	memory_status_graphics debug_ram_display(&text_surface, &nes_cpu_bus, 20 + 512 + 20, 20, "RAM Contents", RAM_ADDRESS_SPACE_START);
	memory_status_graphics debug_rom_display(&text_surface, &nes_cpu_bus, 20 + 512 + 20, 25 + (9 * font_size), "ROM Contents", nes_cpu.get_program_counter()); 
	processor_status_graphics debug_processor_status(&text_surface, &nes_cpu, &nes_ppu, 20 + 512 + 20, 25 + (18 * font_size));
	memory_peek_graphics debug_cpu_memory_peek(&text_surface, &nes_cpu_bus, "CPU Mem Peek", 20 + 512 + 20, 25 + (25 * font_size)); 
	memory_peek_graphics debug_ppu_memory_peek(&text_surface, &nes_ppu_bus, "PPU Mem Peek", 20 + 512 + 20, 25 + (26 * font_size)); 
	pattern_table_preview debug_pattern_table(&nes_ppu_bus, &nes_ppu, renderer, 20 + 512 + 20, 25 + (29 * font_size));
	fps_counter frame_counter(&text_surface, 1280 - (font_size * 6), 0);

	// The main display object
	ppu_draw display_output(&nes_ppu, renderer, 20, 20, 2);

	// SDL event handler, including a keyboard event
	SDL_Event event_handler; 
	SDL_KeyboardEvent *key_event;
	memory_peek_text_input_processor cpu_memory_peek_text_input, ppu_memory_peek_text_input;
	SDL_StopTextInput();	// stop text input by default

	bool quit = false; 

	uint16_t halt_at_pc = 0x0000; // 0x0000 will disable this behaviour
	bool frame_complete = false; 
	bool instruction_complete = false; 
	bool run_mode = true; 		// can be changed to false to pause on initial frame if needs be
	bool debug_displays = true;

	frame_counter.get_frame_start_time();

	//instruction_trace_log.enable_file_logging();		// only enable this for debugging, it will slow emulation down to ~10fps

	while (!quit) { // main application running loop

		/*====================
			Game logic
		  ====================*/

		// process the PPU and CPU as needed by the user

		if (!instruction_complete) {
			if (instruction_trace_log.file_logging_enabled()) {
				// if we are file logging, we want to update this before every PPU/CPU cycle
				instruction_trace_log.update(); // update our trace log, logging to file and also updating the trace log on screen
			}

			nes_ppu.cycle();

			if (nes_ppu.get_clock_pulses() % 4 == 0 || (!run_mode && !instruction_complete))  {	// the CPU cycles once every 4 PPU cycles. The runmode && instruc is to complete the next instruction when paused
				nes_cpu.cycle();
			}				

			if (nes_cpu.finished_instruction() && !run_mode) { // run the cpu until the instruction finishes
				instruction_complete = true;		// instruction complete is not allowed to be toggled if in run mode
			}			

			if (nes_cpu._hit_break == true || nes_cpu.get_program_counter() == halt_at_pc) {   
				std::cout << "Halting at 0x" << std::hex << std::uppercase << nes_cpu.get_program_counter() << std::dec << std::endl;
				run_mode = false;
			}	
		}	

		frame_complete = nes_ppu.get_frame_complete_flag();	

		// if frame count has increased, update everything
		if (frame_complete || instruction_complete) {		

			/*====================
				Process input
			====================*/

			// this might look a bit unconventional, but we only want to check the key input right before rendering the frame. Otherwise this loop runs ~80K times before each frame is rendered. 

			// For gameplay keypresses, we don't want any delay on the keys, so we handle them with a keyboard state, outside of the event handler
			// SDL makes it quite easy for us, to handle this, we just process each key one at a time to load it into the internal shift register
			const Uint8* keystates = SDL_GetKeyboardState(NULL);
			nes_apu_io.process_key(PLAYER_ONE, A_KEY, keystates[SDL_SCANCODE_Z]);		
			nes_apu_io.process_key(PLAYER_ONE, B_KEY, keystates[SDL_SCANCODE_X]);		
			nes_apu_io.process_key(PLAYER_ONE, SEL_KEY, keystates[SDL_SCANCODE_N]);				
			nes_apu_io.process_key(PLAYER_ONE, START_KEY, keystates[SDL_SCANCODE_M]);		
			nes_apu_io.process_key(PLAYER_ONE, UP_KEY, keystates[SDL_SCANCODE_UP]);
			nes_apu_io.process_key(PLAYER_ONE, DOWN_KEY, keystates[SDL_SCANCODE_DOWN]);
			nes_apu_io.process_key(PLAYER_ONE, LEFT_KEY, keystates[SDL_SCANCODE_LEFT]);		
			nes_apu_io.process_key(PLAYER_ONE, RIGHT_KEY, keystates[SDL_SCANCODE_RIGHT]);		
			
			// Handle all events on queue, including the call for quit
			while (SDL_PollEvent(&event_handler)) {		
				if (event_handler.type == SDL_KEYUP) {
					key_event = &event_handler.key;

					switch (key_event->keysym.sym) {
						case SDLK_TAB:	// Enter memory peek mode
							SDL_StartTextInput();
							cpu_memory_peek_text_input.activate();
							ppu_memory_peek_text_input.deactivate();
							debug_cpu_memory_peek.activate_cursor();	
							break;

						case SDLK_F1:	// Enter memory peek mode
							SDL_StartTextInput();
							ppu_memory_peek_text_input.activate();
							cpu_memory_peek_text_input.deactivate();
							debug_ppu_memory_peek.activate_cursor();	
							break;

						case SDLK_RETURN:	// Finish memory peek mode
							if (SDL_IsTextInputActive()) {
								if (cpu_memory_peek_text_input.input_active()) {
									debug_cpu_memory_peek.set_address(cpu_memory_peek_text_input.process());		// if user presses less than 4 chars, pad with zeros and go with what was entered						
									cpu_memory_peek_text_input.deactivate();
								}
								if (ppu_memory_peek_text_input.input_active()) {
									debug_ppu_memory_peek.set_address(ppu_memory_peek_text_input.process());		// if user presses less than 4 chars, pad with zeros and go with what was entered						
									ppu_memory_peek_text_input.deactivate();
								}
								SDL_StopTextInput();
							}
							break;

						case SDLK_F5: // Toggle run mode
							std::cout << "Run mode: " << (run_mode ? "off" : "on") << std::endl;
							run_mode = !run_mode; 
							instruction_complete = false;
							break;

						case SDLK_SPACE:	// Single cycle through CPU
							if (!run_mode) { // cycle the cpu, but only if not in run mode, we don't want to cycle twice in one main loop.
								instruction_complete = false; // prompt the cpu to complete it's next instruction
							}
							break;

						case SDLK_DELETE:		// RESET CPU, PPU and all RAMs
							nes_ram.clear_ram();
							nes_palette_ram.clear_ram();
							nes_cpu.reset();
							nes_ppu.reset();
							instruction_complete = false; // do this so that the processor can progress the first initial clock cycles and pause on the first instruction
							std::cout << "CPU Reset" << std::endl;
							break;

						case SDLK_p:
							debug_pattern_table.select_palette();
							break;

						case SDLK_ESCAPE:
							quit = true;	// quit the program
							break;																	
					}
				}
				else if (event_handler.type == SDL_TEXTINPUT) {
					char key_pressed = event_handler.text.text[0];	

					if (cpu_memory_peek_text_input.input_active()) {
						debug_cpu_memory_peek.input_partial_address(key_pressed);

						if (cpu_memory_peek_text_input.add_character(key_pressed)) {	// returns true if 4 characters have been entered, so that user doesn't have to press enter, they can just pop in their 4 chars
							debug_cpu_memory_peek.set_address(cpu_memory_peek_text_input.process());							
							cpu_memory_peek_text_input.deactivate();
							SDL_StopTextInput();
						}
					}
					if (ppu_memory_peek_text_input.input_active()) {
						debug_ppu_memory_peek.input_partial_address(key_pressed);

						if (ppu_memory_peek_text_input.add_character(key_pressed)) {	// returns true if 4 characters have been entered, so that user doesn't have to press enter, they can just pop in their 4 chars
							debug_ppu_memory_peek.set_address(ppu_memory_peek_text_input.process());							
							ppu_memory_peek_text_input.deactivate();
							SDL_StopTextInput();
						}
					}				
				}
				else if (event_handler.type == SDL_QUIT) {
					quit = true;
				}
			}		

			/*====================
				Display
			====================*/	

			// clear the screen
			SDL_RenderClear(renderer); 	
	
			// draw the debug emulator status displays
			if (!instruction_trace_log.file_logging_enabled()) { // if we are not file logging, we can update this on every frame draw, making this faster
				instruction_trace_log.update();
			}

			if (debug_displays) {
				debug_instr_trace.display_contents();	
				debug_processor_status.display_contents();
				debug_ram_display.display_contents();
				debug_rom_display.display_contents(); 	
				debug_cpu_memory_peek.display_contents();
				debug_ppu_memory_peek.display_contents();
				//debug_pattern_table.display_contents();	
			}
			frame_counter.display_contents();

			display_output.draw();				// draw the main screen
		
			// update the display with new info from renderer
			SDL_RenderPresent(renderer);			
			nes_ppu.clear_frame_complete_flag();
			
			frame_counter.get_frame_start_time();
		}		
	}

	// tidy up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

	return EXIT_SUCCESS;
}