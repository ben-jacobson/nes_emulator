#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include <iostream>
#include <string>

#include "emulator_status_graphics.h"
#include "ram_status_output.h"
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
	std::string font_fullpath = ((std::string)base_path).append("C64_Pro_Mono-STYLE.ttf").c_str();
	emulator_status_graphics test_message(renderer, font_fullpath.c_str(), 14);  
	ram_status_output debug_ram_display(renderer, font_fullpath.c_str(), 14, &nes_ram);

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
		debug_ram_display.display_ram_contents(0, 0);
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

	// tidy up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}