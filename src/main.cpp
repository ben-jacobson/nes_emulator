#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include <iostream>
#include <iomanip>

#include "bus.h"
#include "cpu.h"
#include "ram.h"

constexpr uint16_t SCREEN_WIDTH = 1280;
constexpr uint16_t SCREEN_HEIGHT = 720;

constexpr uint16_t RAM_SIZE_BYTES = 2048; // 2Kb of RAM

SDL_Window *test_window = NULL;

int main()
{
	// Attempt to init SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {        
		std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
		return 0;
    }

    // On success, setup SDL
	SDL_Window* window = SDL_CreateWindow("6502 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // black
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);

	// https://stackoverflow.com/questions/22886500/how-to-render-text-in-sdl2

	bus nes_bus;
	ram nes_ram(&nes_bus, RAM_SIZE_BYTES, 0x000, 0x7FF); // temporarily mapped to 0x000 through to 0x7FF for now
	cpu nes_cpu(&nes_bus, &nes_ram);  // todo, add the PPU

    std::system("clear");

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
	}

//	while (true) {
		// nes_cpu.cycle(); // the cpu will manage it's own clock pulses, and will fetch / decode data as needed. It will also prompt the other devices on the bus to do their thing, e.g just like a real CPU can trigger SS/CE on devices via address bus decoding
//	}
}