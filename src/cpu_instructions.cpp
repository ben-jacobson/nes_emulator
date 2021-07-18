#include "cpu.h"

inline void cpu::check_if_negative(uint8_t data) {
    _status_flags_reg.n = ((data & 0x80) >> 7) == 1 ? 1 : 0;
}

inline void cpu::check_if_zero(uint8_t data) {
    _status_flags_reg.z = data == 0 ? 1: 0;
}

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
    return 0;
}

uint8_t cpu::instr_JSR(void) {
    return 0;
}

uint8_t cpu::instr_LDA(void) {
    return 0;
}

uint8_t cpu::instr_LDX(void) {
    return 0;
}

uint8_t cpu::instr_LDY(void) {
    return 0;
}

uint8_t cpu::instr_LSR(void) {
    return 0;
}

uint8_t cpu::instr_NOP(void) {
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
    return 0;
}

uint8_t cpu::instr_STX(void) {
    return 0;
}

uint8_t cpu::instr_STY(void) {
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
    return 0;
}