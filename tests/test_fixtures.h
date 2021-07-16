#pragma once

#include <functional>

#include "memory_map.h"
#include "bus.h"
#include "cpu.h"
#include "ram.h"
#include "cartridge.h"

// test fixtures
/* extern bus test_bus;
extern ram test_ram;
extern cpu test_cpu; 
extern cartridge test_cart;

void hack_in_test_rom_data(uint16_t address, uint8_t data);

*/ // moved int actual text fixture class

class emulator_test_fixtures {
public:
    bus *test_bus;
    ram *test_ram;
    ram *another_test_ram;
    cartridge *test_cart;
    cpu *test_cpu;

    //emulator_test_fixtures();
    //~emulator_test_fixtures();
    //void hack_in_test_rom_data(uint16_t address, uint8_t data);

    emulator_test_fixtures() {
        test_bus = new bus();
        test_ram = new ram(test_bus, RAM_SIZE_BYTES, RAM_ADDRESS_SPACE_START, RAM_ADDRESS_SPACE_END);
        test_cart = new cartridge(test_bus, CART_ADDRESS_SPACE_START, CART_ADDRESS_SPACE_END);
        test_cpu = new cpu(test_bus, test_ram);

        // register some test ram device
        //test_bus.register_new_bus_device(RAM_ADDRESS_SPACE_START, RAM_ADDRESS_SPACE_END, std::bind(&ram::read, &test_ram, std::placeholders::_1), std::bind(&ram::write, &test_ram, std::placeholders::_1, std::placeholders::_2));    
        test_bus->register_new_bus_device(RAM_ADDRESS_SPACE_START, RAM_ADDRESS_SPACE_END, test_ram->_read_function_ptr, test_ram->_write_function_ptr);    

        // create an additional device for testing and register it
        another_test_ram = new ram(test_bus, RAM_SIZE_BYTES, RAM_MIRRORA_SPACE_START, RAM_MIRRORA_SPACE_END); 
        //test_bus.register_new_bus_device(RAM_MIRRORA_SPACE_START, RAM_MIRRORA_SPACE_END, std::bind(&ram::read, &another_test_ram, std::placeholders::_1), std::bind(&ram::write, &another_test_ram, std::placeholders::_1, std::placeholders::_2));    
        test_bus->register_new_bus_device(RAM_MIRRORA_SPACE_START, RAM_MIRRORA_SPACE_END, another_test_ram->_read_function_ptr, another_test_ram->_write_function_ptr);    

        //test_bus.register_new_bus_device(CART_ADDRESS_SPACE_START, CART_ADDRESS_SPACE_END, std::bind(&cartridge::read, &test_cart, std::placeholders::_1));    
        test_bus->register_new_bus_device(CART_ADDRESS_SPACE_START, CART_ADDRESS_SPACE_END, test_cart->_read_function_ptr);    
    }

    ~emulator_test_fixtures() {
        delete test_bus;
        delete test_ram;
        delete another_test_ram;
        delete test_cart;
        delete test_cpu;
    }

    void hack_in_test_rom_data(uint16_t relative_address, uint8_t data) {
        // hacks some data into the rom via it's rom pointer. Uses relative addresses, i.e not in the global address space. Address 0x0000 is is address 0x0000 
        uint8_t* cart_ptr = test_cart->get_rom_pointer();    
        *(cart_ptr + relative_address) = data;
    }    
};

