#include <iostream>

#include "bus.h"
#include "cpu.h"
#include "ram.h"

int main() 
{
	bus nes_bus;
	ram nes_ram(&nes_bus);
	cpu nes_cpu(&nes_bus, &nes_ram);  // todo, add the PPU

	std::cout << "Computer is running!" << std::endl;

	while (true) {
		nes_cpu.cycle(); // the cpu will manage it's own clock pulses, and will fetch / decode data as needed. It will also prompt the other devices on the bus to do their thing, e.g just like a real CPU can trigger SS/CE on devices via address bus decoding
	}
}

