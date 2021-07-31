#include "cpu.h"

// every address mode assumes the opcode has already been grabbed, the program counter has been incremented and we are now reading the second byte
// there is no need to increment the program counter after this, the cpu cycle function will handle this. 
// every function must set the _fetched_address value to an address

uint8_t cpu::addr_mode_ABS(void) {
    // In absolute address mode, the second byte of the instruction is the lower 8 bits of the address, with the third byte being the upper 8 bits of the address (little endian)
    // This allows an instruction to apply to the entire 64Kb range of memory. 
    _bus_ptr->set_address(_program_counter);
    uint8_t low = _bus_ptr->read_data();
    _program_counter++;

    _bus_ptr->set_address(_program_counter);
    uint8_t high = _bus_ptr->read_data();

    _fetched_address = (high << 8) | low;               // a memory address is fetched
    _program_counter++;
    return 0; 
}

uint8_t cpu::addr_mode_ABSX(void) {
    _bus_ptr->set_address(_program_counter);
    uint8_t low = _bus_ptr->read_data();
    _program_counter++;

    _bus_ptr->set_address(_program_counter);
    uint8_t high = _bus_ptr->read_data();

    _fetched_address = (high << 8) | low;            
    _program_counter++;

    _fetched_address += _x_index_reg;                   // add x index to the fetched address. A memory address is fetched

    if ((_fetched_address & 0xFF00) != (high << 8)) {       // now that we have added our address offset, check to see if we have crossed page boundary, in which case add an additional clock cycle
        return 1; 
    }

    return 0;
}

uint8_t cpu::addr_mode_ABSY(void) {
    _bus_ptr->set_address(_program_counter);
    uint8_t low = _bus_ptr->read_data();
    _program_counter++;

    _bus_ptr->set_address(_program_counter);
    uint8_t high = _bus_ptr->read_data();

    _fetched_address = (high << 8) | low;               // a memory address is fetched
    _program_counter++;

    _fetched_address += _y_index_reg;                   // a memory address is fetched

    if ((_fetched_address & 0xFF00) != (high << 8)) {       // now that we have added our address offset, check to see if we have crossed page boundary, in which case add an additional clock cycle
        return 1; 
    }    
    return 0;
}

uint8_t cpu::addr_mode_ACC(void) {
    // this address mode enacts it's instructions on the accumulator,
    // e.g ASL, LSR, ROL and ROR
    // All ACCUM address mode op codes require 2 clock cycles and every opcode will know to operate on the ACC, hence we return 0 so as to add no clock cycles
    _accumulator_addressing_mode = true;    // sets this flag to morph the instruction behavior
    return 0;       // nothing to fetch.
}

uint8_t cpu::addr_mode_IMM(void) {
    _fetched_address = _program_counter;      // the second byte is the operand being acted upon. the address of the operand is fetched
    _program_counter++;
    return 0; 
}

uint8_t cpu::addr_mode_INDI(void) {
    // In indirect address mode, this works a bit like a pointer
    // it grabs the address first just like you would in ABS
    // then the data from that location becomes the new address
    //uint16_t indirect_address = 0;
    
    _bus_ptr->set_address(_program_counter);
    uint8_t low = _bus_ptr->read_data();
    _program_counter++;

    _bus_ptr->set_address(_program_counter);
    uint8_t high = _bus_ptr->read_data();

    _program_counter = (high << 8) | low;       // we've just read the address from the instructions 2nd and 3rd byte
    _bus_ptr->set_address(_program_counter); 
    low = _bus_ptr->read_data();

    _program_counter++;
    _bus_ptr->set_address(_program_counter);    
    high = _bus_ptr->read_data();

    _fetched_address = (high << 8) | low;               // the address points to a new address. This address is fetched
    _program_counter++;   
    return 0; 
}

uint8_t cpu::addr_mode_INDX(void) {
    _bus_ptr->set_address(_program_counter);
    // grab the address offset from the operand, add this to the x index but discard the carry. 
    uint8_t indirect_address = (_bus_ptr->read_data() + _x_index_reg);     // forcing this into an 8 bit number will cause it to wrap around
    
    // both addresses are expected to be zero page, so also discard the top 8 bits
    _bus_ptr->set_address((indirect_address) & 0x00FF); 
    uint8_t low = _bus_ptr->read_data();    
    _bus_ptr->set_address((indirect_address + 1) & 0x00FF); 
    uint8_t high = _bus_ptr->read_data();   
    _fetched_address = (high << 8) | low;       
    _program_counter++;   
    return 0; 
}

uint8_t cpu::addr_mode_INDY(void) {
    _bus_ptr->set_address(_program_counter);
    // grab the address offset from the operand, add this to the y index. 
    uint8_t operand = _bus_ptr->read_data();
    int carry = (operand + _y_index_reg) > 255 ? 1 : 0;
    uint8_t indirect_address = operand + _y_index_reg;   // forcing this into an 8 bit number will cause it to wrap around

    // both addresses are expected to be zero page, so also discard the top 8 bits
    _bus_ptr->set_address((indirect_address) & 0x00FF); 
    uint8_t low = _bus_ptr->read_data();    
    _bus_ptr->set_address((indirect_address + 1) & 0x00FF); 
    uint8_t high = _bus_ptr->read_data() + carry;       // add the carry to the high order bit
    _fetched_address = (high << 8) | low;       
    _program_counter++;   
    return 0; 
}

uint8_t cpu::addr_mode_IMP(void) {
    // in implied address mode, the address containing the operant is implicitly stated in the operation code of instruction. There are no additional clock cycles to add.
    return 0;
}

uint8_t cpu::addr_mode_REL(void) {
    _bus_ptr->set_address(_program_counter);
    uint8_t offset = _bus_ptr->read_data();
    _program_counter++;

    // we could just cast the offset to an int8_t but wheres the fun in that?? 
    if (offset >> 7 == 1) { // check if MSB is a 1
        offset = ~offset; // invert it
        offset += 1;    // and add one to get the unsigned value
        _fetched_address = _program_counter - offset;           // a new PC value is fetched, this is a special use case since only jump instructions use this
    }
    else {
        _fetched_address = _program_counter + offset;           // a new PC value is fetched
    }

    return 0; // todo
}

uint8_t cpu::addr_mode_ZP(void) {
    // this address mode takes only the second byte as it's address, and assumes that the third byte is all zero. in hardware this is faster, but probably doens't make much difference to an emulator
    _bus_ptr->set_address(_program_counter);
    _fetched_address = (_bus_ptr->read_data()) & 0x00FF;        // a memory address is fetched
    _program_counter++;
    return 0;
}

uint8_t cpu::addr_mode_ZPX(void) {
    // Index zero page addressing calculates it's address by adding the second byte to the x_index register. 
    _bus_ptr->set_address(_program_counter);
    _fetched_address = (_bus_ptr->read_data() + _x_index_reg) & 0x00FF;     // a memory address is fetched
    _program_counter++; 
    return 0; 
}

uint8_t cpu::addr_mode_ZPY(void) {
    // Index zero page addressing calculates it's address by adding the second byte to the y_index register. 
    _bus_ptr->set_address(_program_counter);
    _fetched_address = (_bus_ptr->read_data() + _y_index_reg) & 0x00FF;     // a memory address is fetched
    _program_counter++;
    return 0;
}