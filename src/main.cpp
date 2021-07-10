#include <iostream>
#include <iomanip>

#include "bus.h"
#include "cpu.h"
#include "ram.h"

constexpr uint16_t RAM_SIZE_BYTES = 2048; // 2Kb of RAM

int main() 
{
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

