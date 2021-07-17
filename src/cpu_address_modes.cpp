#include "cpu.h"

uint8_t cpu::addr_mode_ABS(void) {
    // In absolute address mode, the second byte of the instruction is the lower 8 bits of the address, with the third byte being the upper 8 bits of the address (little endian)
    // This means that an instruction can apply to the entire 64Kb range of memory. 
    // ...assuming the opcode has already been grabbed and we are ready to move forward with the PC
    uint16_t low = _program_counter;   
    _program_counter++;

    uint16_t high = _program_counter;
    _program_counter++;

    _fetched_address = (high << 8) | low;
    return 0; 
}

uint8_t cpu::addr_mode_ABSX(void) {
    return 0; // todo
}

uint8_t cpu::addr_mode_ABSY(void) {
    return 0; // todo
}

uint8_t cpu::addr_mode_ACCUM(void) {
    // this address mode enacts it's instructions on the accumulator,
    // e.g ASL, LSR, ROL and ROR
    // All ACCUM address mode op codes require 2 clock cycles and every opcode will know to operate on the ACC, hence we return 0 so as to add no clock cycles
    return 0; 
}

uint8_t cpu::addr_mode_IMM(void) {
    // the second byte of the instruction contains the operand, with no further memory addressing required. Return zero after incrementing the program counter    
    _bus_ptr->set_address(_program_counter);
    _fetched_operand = _bus_ptr->read_data(); // grab a copy of the operand from the program counter directly, then increment.
    _program_counter++;
    return 0; // todo
}

uint8_t cpu::addr_mode_INDI(void) {
    return 0; // todo
}

uint8_t cpu::addr_mode_INDX(void) {
    return 0; // todo
}

uint8_t cpu::addr_mode_INDY(void) {
    return 0; // todo
}

uint8_t cpu::addr_mode_IMP(void) {
    // in implied address mode, the address containing the operant is implicitly stated in the operation code of instruction. There are no additional clock cycles to add.
    return 0;
}

uint8_t cpu::addr_mode_REL(void) {
    return 0; // todo
}

uint8_t cpu::addr_mode_ZP(void) {
    // this address mode takes only the second byte as it's address, and assumes that the third byte is all zero. in hardware this is faster, but probably doens't make much difference to an emulator
    _bus_ptr->set_address(_program_counter);
    _fetched_address = (_bus_ptr->read_data()) & 0x00FF;
    _program_counter++;
    return 0;
}

uint8_t cpu::addr_mode_ZPX(void) {
    // Index zero page addressing calculates it's address by adding the second byte to the x_index register. 
    _bus_ptr->set_address(_program_counter);
    _fetched_address = (_bus_ptr->read_data() + _x_index_reg) & 0x00FF;
    _program_counter++;
    return 0; 
}

uint8_t cpu::addr_mode_ZPY(void) {
    // Index zero page addressing calculates it's address by adding the second byte to the y_index register. 
    _bus_ptr->set_address(_program_counter);
    _fetched_address = (_bus_ptr->read_data() + _y_index_reg) & 0x00FF;
    _program_counter++;
    return 0;
}