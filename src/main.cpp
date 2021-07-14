#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <iostream>
#include <iomanip>
#include <string>

#include "emulator_status_graphics.h"
#include "bus.h"
#include "cpu.h"
#include "ram.h"

int main()
{
	constexpr uint16_t SCREEN_WIDTH = 1280;
	constexpr uint16_t SCREEN_HEIGHT = 720;

	constexpr uint16_t RAM_SIZE_BYTES = 2048; // 2Kb of RAM for emulator

	// initialize our bus, ram and cpu
	bus nes_bus;
	ram nes_ram(&nes_bus, RAM_SIZE_BYTES, 0x000, 0x7FF); // temporarily mapped to 0x000 through to 0x7FF for now
	cpu nes_cpu(&nes_bus, &nes_ram);  // todo, add the PPU

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

	// initialize our emulator status graphics devices
	emulator_status_graphics test_message(renderer, ((std::string)base_path).append("C64_Pro_Mono-STYLE.ttf").c_str(), 14);  // it looks messy, but I don't want the file path variable sitting on the stack for the rest of the programs execution unnecessarily

	SDL_Event event_handler; 
	bool quit = false; 

	uint16_t x_pos = 100, y_pos = 100; 
	int8_t x_speed = 2, y_speed = 2; 
    std::string text_to_render; 

	while (!quit) { // main application running loop
		while (SDL_PollEvent(&event_handler) != 0) {//Handle events on queue
			if (event_handler.type == SDL_QUIT) {			//User requests quit
				quit = true;
			}
		}

		SDL_RenderClear(renderer); // clear the screen
		text_to_render = "x: " + std::to_string(x_pos) + ", y: " + std::to_string(y_pos);
		test_message.draw_to_buffer(text_to_render);		
		SDL_RenderPresent(renderer);	// update the display with new info from renderer
		SDL_Delay(16); // Cap to roughly 60 FPS, we'll work out something a bit more official shortly. 

		test_message.set_position(x_pos, y_pos);
		x_pos += x_speed;  //controls the rect's x coordinate 
		y_pos += y_speed;  // controls the rect's y coordinate		

		if (x_pos <= 0 || x_pos + test_message.get_text_width(text_to_render) >= SCREEN_WIDTH) {
			x_speed = -x_speed;
		}

		if (y_pos <= 0 || y_pos + test_message.get_text_height(text_to_render) >= SCREEN_HEIGHT) {
			y_speed = -y_speed;
		}		
	}

    /* std::system("clear");

	// Output ram content, with the first column being address offset. 
	std::cout << "RAM contents:" << std::endl; // 6 spaces to offset the header row.
	std::cout.setf(std::ios::hex, std::ios::basefield);
	std::cout << std::endl << "0x00: "; 

	for (uint16_t i = 0; i < 64; i++) { // show the first 64 bytes of RAM
		if (i > 0 && i % 8 == 0) {
			std::cout << std::endl; // insert newline every 8 characters. 			
			std::cout << "0x" << std::setfill('0') << std::setw(2) << i << ": "; 			
		}		
		std::cout << std::setfill('0') << std::setw(2) << (uint16_t)(nes_ram.debug_read(i)) << " ";  // seems that we need to cast to uint16_t to allow the fill and width to work properly. 
	}*/ 

	// tidy up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}