#include "apu_io.h"

apu_io::apu_io(uint16_t address_space_lower, uint16_t address_space_upper)
    : bus_device()
{
    _address_space_lower = address_space_lower;
    _address_space_upper = address_space_upper;    

    controller_register_one = 0xFF;
    controller_register_two = 0xFF; // all controls held high

    _strobe = 8;  // start this at the highest value, indicating we have finished strobing / not yet strobing
}

uint8_t apu_io::read(uint16_t address) {
    uint8_t data = BUTTON_UP;   // don't accidentally trigger the key press

    if (address == PLAYER_ONE_CONTROLLER_REG || address == PLAYER_TWO_CONTROLLER_REG) {
        uint8_t* controller_reg_ptr = (address == PLAYER_ONE_CONTROLLER_REG ? &controller_register_one : &controller_register_two);

        if (_strobe < 8) {
            data = check_bit(*controller_reg_ptr, _strobe);
            _strobe++;   
        }
        else {            
            // the NES would continually return the A key status if we haven't written our strobe value yet
            data = check_bit(*controller_reg_ptr, 0);
        }
    }
    return data;
}

void apu_io::write(uint16_t address, uint8_t data) {
    /*  
        Here we are simulating the effect of the shift register internal to the controller.
        writing to the LSB will simulate the strobe, 
        A sequence of 1 then 0 will commence the shift register
    */

    if (address == PLAYER_ONE_CONTROLLER_REG) {
        _strobe = 8; // leave this at the end of the strobe

        if (check_bit(data, 0) == 1) {  // check if the polling bit was set 
            _strobe = 0;    // set strobe to zero, indicating we can start a new strobe
        }
    } 
}

void apu_io::process_key(uint8_t player, uint8_t key, uint8_t state) {
    // determine which controller register we are changing the value of
    uint8_t* controller_reg_ptr = (player == PLAYER_ONE ? &controller_register_one : &controller_register_two);
    // set or clear the appropriate bit based on state
    *controller_reg_ptr ^= (-state ^ *controller_reg_ptr) & (1UL << key);       // Whoa there cowboy!!
}

void apu_io::debug_set_key_register(uint8_t player, uint8_t value) {
    uint8_t* controller_reg_ptr = (player == PLAYER_ONE ? &controller_register_one : &controller_register_two);
    *controller_reg_ptr = value;
}

uint8_t apu_io::debug_get_key_register(uint8_t player) {
    return (player == PLAYER_ONE ? controller_register_one : controller_register_two);
}