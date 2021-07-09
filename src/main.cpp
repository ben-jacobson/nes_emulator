#include <iostream>

#include "bus.h"
#include "cpu.h"
#include "ram.h"

int main() 
{
	bus nes_bus;
	cpu nes_2A03(&nes_bus);
	ram nes_ram(&nes_bus);

	std::cout << "Computer is running!" << std::endl;
}