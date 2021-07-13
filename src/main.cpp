#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <iostream>
#include <iomanip>
#include <string>

#include "bus.h"
#include "cpu.h"
#include "ram.h"

constexpr uint16_t SCREEN_WIDTH = 1280;
constexpr uint16_t SCREEN_HEIGHT = 720;

constexpr uint16_t RAM_SIZE_BYTES = 2048; // 2Kb of RAM

SDL_Window *test_window = NULL;

int main()
{
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
    } 

	// std::cout << "Base path set at: " << test << std::endl;

    // On success, setup SDL window and rendered
	SDL_Window* window = SDL_CreateWindow("6502 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // black, full alpha
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);

	// initialize our TTF font
	TTF_Init();
	std::string font_file_name = ((std::string)base_path).append("FreeSans.ttf");		// a bit of a casting nightmare to say the least, but it works. 
	TTF_Font* Sans_font = TTF_OpenFont(font_file_name.c_str(), 24);

    if (Sans_font == NULL) {
		std::cout << "error - font not found, error code: " << TTF_GetError() << std::endl;    	 
	    exit(EXIT_FAILURE);
    }

	SDL_Color White = {255, 255, 255, 255}; // full white, full alpha
	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans_font, "Hello SDL!", White); 
	
	// Convert the message into a texture
	SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

	SDL_Rect Message_rect; //create a rect
	Message_rect.x = 10;  //controls the rect's x coordinate 
	Message_rect.y = 10; // controls the rect's y coordinte
	Message_rect.w = 110; // controls the width of the rect
	Message_rect.h = 110; // controls the height of the rect

	bool quit = false; //Main loop flag
	SDL_Event event_handler; //Event handler

	//While application is running
	while (!quit) {
		while (SDL_PollEvent(&event_handler) != 0) {//Handle events on queue
			if (event_handler.type == SDL_QUIT) {			//User requests quit
				quit = true;
			}
		}

		SDL_RenderClear( renderer );
		SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
		SDL_RenderPresent(renderer);
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
	TTF_CloseFont(Sans_font);

	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(Message);
	TTF_Quit();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}