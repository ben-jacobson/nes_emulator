#include "catch.hpp"
#include "test_fixtures.h"
    
TEST_CASE_METHOD(emulator_test_fixtures, "apu_io - Test process key", "[apu_io]") {
    // alter the register values and determine if it worked
    test_apu_io.process_key(PLAYER_ONE, RIGHT_KEY, BUTTON_DOWN);
    uint8_t io_result = test_apu_io.debug_get_key_register(PLAYER_ONE);
    CHECK(io_result == 0x7F);

    // and again for good measure
    test_apu_io.process_key(PLAYER_TWO, A_KEY, BUTTON_DOWN);
    io_result = test_apu_io.debug_get_key_register(PLAYER_TWO);
    CHECK(io_result == 0xFE); 
}

TEST_CASE_METHOD(emulator_test_fixtures, "apu_io - Test debug get key", "[apu_io]") {
    // start by setting all keys high for player one in case another test has altered something
    test_apu_io.debug_set_key_register(PLAYER_ONE, 0xFF);
    test_apu_io.debug_set_key_register(PLAYER_TWO, 0xFF);

    // we expect that the IO registers are set to 0xFF
    uint8_t io_result = test_apu_io.debug_get_key_register(PLAYER_ONE);
    CHECK(io_result == 0xFF); 
    io_result = test_apu_io.debug_get_key_register(PLAYER_TWO);
    CHECK(io_result == 0xFF);            
}

TEST_CASE_METHOD(emulator_test_fixtures, "apu_io - Test key strobing behaviour when strobe bit is not set", "[apu_io]") {
    uint8_t result;

    // start by setting all keys high for player one in case another test has altered something
    test_apu_io.debug_set_key_register(PLAYER_ONE, 0xFF);

    test_bus.set_address(PLAYER_ONE_CONTROLLER_REG);
    test_bus.write_data(0); // start by deliberately not setting the strobe, we should essentially just be reading the A key over and over now. 

    // process 8 reads, one for each key, they should all be HIGH / 1
    for (uint8_t i = 0; i < RIGHT_KEY; i++) {
        result = test_bus.read_data();
        CHECK(result == BUTTON_UP);
    }

    // try this exercise one more time, except set a few keys, we should see all highs, except for the A key
    test_apu_io.process_key(PLAYER_ONE, A_KEY, BUTTON_UP);
    test_apu_io.process_key(PLAYER_ONE, UP_KEY, BUTTON_DOWN);
    test_apu_io.process_key(PLAYER_ONE, RIGHT_KEY, BUTTON_DOWN);
    test_apu_io.process_key(PLAYER_ONE, SEL_KEY, BUTTON_DOWN);

    // test that each button is read as button up, except A. This could have been a loop, but the error reporting would not show which cases failed that way.
    for (uint8_t i = 0; i < RIGHT_KEY; i++) {
        result = test_bus.read_data();  
        CHECK(result == BUTTON_UP); // we should only be reading the value of A key
    }

    test_apu_io.process_key(PLAYER_ONE, A_KEY, BUTTON_DOWN);
    test_bus.set_address(PLAYER_ONE_CONTROLLER_REG);
    test_bus.write_data(0); // start by deliberately not setting the strobe, we should essentially just be reading the A key over and over now. 

    for (uint8_t i = 0; i < RIGHT_KEY; i++) {
        result = test_bus.read_data();  
        CHECK(result == BUTTON_DOWN); // once again we should only be reading the value of A key
    }
}

TEST_CASE_METHOD(emulator_test_fixtures, "apu_io - Test key strobing behaviour when strobe bit is set", "[apu_io]") {
    uint8_t result;

    // start by setting all keys high for player one in case another test has altered something
    test_apu_io.debug_set_key_register(PLAYER_ONE, 0xFF);

    // set up a few keys to known values
    test_apu_io.process_key(PLAYER_ONE, A_KEY, BUTTON_DOWN);
    test_apu_io.process_key(PLAYER_ONE, UP_KEY, BUTTON_DOWN);
    test_apu_io.process_key(PLAYER_ONE, SEL_KEY, BUTTON_DOWN);

    test_bus.set_address(PLAYER_ONE_CONTROLLER_REG);
    test_bus.write_data(0x09); // start by deliberately setting the strobe with any value with LSB showing 1, this enables us to strobe our keys from the register 

    result = test_bus.read_data(); // A
    CHECK(result == BUTTON_DOWN);
    result = test_bus.read_data(); // B
    CHECK(result == BUTTON_UP);
    result = test_bus.read_data(); // SEL
    CHECK(result == BUTTON_DOWN);    
    result = test_bus.read_data(); // START
    CHECK(result == BUTTON_UP);    
    result = test_bus.read_data(); // UP
    CHECK(result == BUTTON_DOWN);    
    result = test_bus.read_data(); // DOWN
    CHECK(result == BUTTON_UP);    
    result = test_bus.read_data(); // LEFT
    CHECK(result == BUTTON_UP);    
    result = test_bus.read_data(); // RIGHT
    CHECK(result == BUTTON_UP);    

    // since we have finished our strobe, all consective reads after this will just return the value of A key

    result = test_bus.read_data(); // A
    CHECK(result == BUTTON_DOWN);
    result = test_bus.read_data(); // A
    CHECK(result == BUTTON_DOWN);
    test_apu_io.process_key(PLAYER_ONE, A_KEY, BUTTON_UP);
    result = test_bus.read_data(); // A
    CHECK(result == BUTTON_UP);            
}