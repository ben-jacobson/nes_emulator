#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include <iostream>
#include <string>

#include "memory_map.h"
#include "status_graphics.h"
#include "memory_status_graphics.h"
#include "bus.h"
#include "cpu.h"
#include "ram.h"
#include "cartridge.h"

int main()
{
	constexpr uint16_t SCREEN_WIDTH = 1280;
	constexpr uint16_t SCREEN_HEIGHT = 720;

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

	// create a placeholder rectangle for when we eventually want to render some display
	SDL_Surface* placeholder_game_display_area = SDL_CreateRGBSurface(0, 256 * 4, 240 * 4, 32, 0, 0, 0, 0);

	if (placeholder_game_display_area == NULL) {
		std::cout << "CreateRGBSurface failed: " << SDL_GetError() << std::endl;
		exit(1);
	}

	SDL_Rect placeholder_game_display_rect;
	placeholder_game_display_rect.x = 20;
	placeholder_game_display_rect.y = 20;
	placeholder_game_display_rect.w = 256 * 4; // scaled up to 4x
	placeholder_game_display_rect.h = 240 * 4;
	SDL_FillRect(placeholder_game_display_area, NULL, SDL_MapRGB(placeholder_game_display_area->format, 255, 255, 255));
	SDL_Texture* placeholder_game_display = SDL_CreateTextureFromSurface(renderer, placeholder_game_display_area);
	
	// initialize our font object
    if(!TTF_WasInit() && TTF_Init() != 0) {
        std::cout << "TTF_Init unsuccessful: " << TTF_GetError() << std::endl;
        exit(EXIT_FAILURE);
	}

	uint8_t font_size = 14; 
	std::string font_fullpath = ((std::string)base_path).append("C64_Pro_Mono-STYLE.ttf").c_str();
	status_graphics test_message(renderer, font_fullpath.c_str(), font_size);  
	memory_status_graphics debug_ram_display("RAM Contents:", RAM_ADDRESS_SPACE_START, renderer, 0, 0, font_fullpath.c_str(), font_size, &nes_bus);
	memory_status_graphics debug_rom_display("ROM Contents:", ROM_ADDRESS_SPACE_START, renderer, 0, 18 * font_size, font_fullpath.c_str(), font_size, &nes_bus);

	SDL_Event event_handler; 
	bool quit = false; 

	uint16_t x_pos = 100, y_pos = 100; 
	int8_t x_speed = 2, y_speed = 2; 
    std::string text_to_render; 

	while (!quit) { // main application running loop
		while (SDL_PollEvent(&event_handler) != 0) {		// Handle events on queue
			if (event_handler.type == SDL_QUIT) {			// If user requests quit
				quit = true;
			}
		}

		SDL_RenderClear(renderer); // clear the screen
		
		//draw the game area placeholder
		SDL_RenderCopy(renderer, placeholder_game_display, NULL, &placeholder_game_display_rect); // copy any new textures to the renderer

		
		text_to_render = "x: " + std::to_string(x_pos) + ", y: " + std::to_string(y_pos);
		test_message.draw_to_buffer(text_to_render);		
		debug_ram_display.display_contents();
		debug_rom_display.display_contents(); 

		// nes_cpu.cycle();

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
	SDL_FreeSurface(placeholder_game_display_area);
	SDL_DestroyTexture(placeholder_game_display);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}