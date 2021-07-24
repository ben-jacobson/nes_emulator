#include "cpu.h"

uint8_t cpu::instr_ADC(void) {
    return 0;
}

uint8_t cpu::instr_AND(void) {
    return 0;
}

uint8_t cpu::instr_ASL(void) {
    return 0;
}

uint8_t cpu::instr_BCC(void) {
    return 0;
}

uint8_t cpu::instr_BCS(void) {
    return 0;
}

uint8_t cpu::instr_BEQ(void) {
    return 0;
}

uint8_t cpu::instr_BIT(void) {
    return 0;
}

uint8_t cpu::instr_BMI(void) {
    return 0;
}                                          

uint8_t cpu::instr_BNE(void) {
    return 0;
}

uint8_t cpu::instr_BPL(void) {
    return 0;
}

uint8_t cpu::instr_BRK(void) {
    program_counter_to_stack(2); // PC+2 is pushed to stack
    push_to_stack(get_status_flags());
    _status_flags_reg.b = 1; 
    return 0;
}

uint8_t cpu::instr_BVC(void) {
    return 0;
}

uint8_t cpu::instr_BVS(void) {
    return 0;
}

uint8_t cpu::instr_CLC(void) {
    return 0;
}                                              
uint8_t cpu::instr_CLD(void) {
    return 0;
}

uint8_t cpu::instr_CLI(void) {
    _status_flags_reg.i = 0;
    return 0;
}

uint8_t cpu::instr_CLV(void) {
    return 0;
}

uint8_t cpu::instr_CMP(void) {
    return 0;
}

uint8_t cpu::instr_CPX(void) {
    return 0;
}

uint8_t cpu::instr_CPY(void) {
    return 0;
}

uint8_t cpu::instr_DEC(void) {
    _bus_ptr->set_address(_fetched);
    uint8_t value = _bus_ptr->read_data();
    _bus_ptr->write_data(value - 1);    
    return 0;
}

uint8_t cpu::instr_DEX(void) {
    _x_index_reg--;
    check_if_negative(_x_index_reg);
    check_if_zero(_x_index_reg);
    return 0;
}

uint8_t cpu::instr_DEY(void) {
    _y_index_reg--;
    check_if_negative(_y_index_reg);
    check_if_zero(_y_index_reg);
    return 0;
}

uint8_t cpu::instr_EOR(void) {
    return 0;
}

uint8_t cpu::instr_INC(void) {
    _bus_ptr->set_address(_fetched);
    uint8_t value = _bus_ptr->read_data();
    _bus_ptr->write_data(value + 1);
    return 0;
}

uint8_t cpu::instr_INX(void) {
    _x_index_reg++;
    check_if_negative(_x_index_reg);
    check_if_zero(_x_index_reg);
    return 0;
}

uint8_t cpu::instr_INY(void) {
    _y_index_reg++;
    check_if_negative(_y_index_reg);
    check_if_zero(_y_index_reg); 
    return 0;
}

uint8_t cpu::instr_JMP(void) {
    set_program_counter(_fetched);
    return 0;
}

uint8_t cpu::instr_JSR(void) {
    return 0;
}

uint8_t cpu::instr_LDA(void) {
    _accumulator_reg = (_fetched & 0xFF);       // we can only accept the lower 8 bits
    check_if_negative(_accumulator_reg);
    check_if_zero(_accumulator_reg);      
    return 0;
}

uint8_t cpu::instr_LDX(void) {
    _x_index_reg = (_fetched & 0xFF);       // we can only accept the lower 8 bits
    check_if_negative(_x_index_reg);
    check_if_zero(_x_index_reg);  
    return 0;
}

uint8_t cpu::instr_LDY(void) {
    _y_index_reg = (_fetched & 0xFF);       // we can only accept the lower 8 bits
    check_if_negative(_y_index_reg);
    check_if_zero(_y_index_reg);      
    return 0;
}

uint8_t cpu::instr_LSR(void) {
    return 0;
}

uint8_t cpu::instr_NOP(void) {
    // do nothing
    return 0;
}

uint8_t cpu::instr_ORA(void) {
    return 0;
}

uint8_t cpu::instr_PHA(void) {
    return 0;
}

uint8_t cpu::instr_PHP(void) {
    push_to_stack(get_status_flags());
    return 0;   // no more cycles needed
}

uint8_t cpu::instr_PLA(void) {
    return 0;
}

uint8_t cpu::instr_PLP(void) {
    return 0;
}

uint8_t cpu::instr_ROL(void) {
    return 0;
}

uint8_t cpu::instr_ROR(void) {
    return 0;
}

uint8_t cpu::instr_RTI(void) {
    return 0;
}

uint8_t cpu::instr_RTS(void) {
    return 0;
}

uint8_t cpu::instr_SBC(void) {
    return 0;
}

uint8_t cpu::instr_SEC(void) {
    return 0;
}

uint8_t cpu::instr_SED(void) {
    return 0;
}

uint8_t cpu::instr_SEI(void) {
    _status_flags_reg.i = 1;
    return 0;
}

uint8_t cpu::instr_STA(void) {
    _bus_ptr->set_address(_fetched);
    _bus_ptr->write_data(_accumulator_reg); 
    return 0;
}

uint8_t cpu::instr_STX(void) {
    _bus_ptr->set_address(_fetched);
    _bus_ptr->write_data(_x_index_reg);
    return 0;
}

uint8_t cpu::instr_STY(void) {
    _bus_ptr->set_address(_fetched);
    _bus_ptr->write_data(_y_index_reg);  
    return 0;
}

uint8_t cpu::instr_TAX(void) {
    return 0;
}

uint8_t cpu::instr_TAY(void) {
    return 0;
}

uint8_t cpu::instr_TSX(void) {
    return 0;
}

uint8_t cpu::instr_TXA(void) {
    return 0;
}

uint8_t cpu::instr_TXS(void) {
    return 0;
}

uint8_t cpu::instr_TYA(void) {
    return 0;
}

uint8_t cpu::instr_ZZZ(void) {
    // do nothing
    return 0;
}