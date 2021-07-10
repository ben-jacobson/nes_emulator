#include "cpu.h"

cpu::cpu(bus *bus_ptr, ram *ram_ptr) 
:   bus_device(bus_ptr)
{
    _ram_ptr = ram_ptr; 
    reset();
} 

void cpu::cycle(void) {
    // fetch data
    // a cycle can have multiple clock cycles, this is affected by how the instructions are morphed by the addressing modes. 
}

void cpu::clock(void) {

}

void cpu::reset(void) {

} 

void cpu::IRQ(void) {

}

void cpu::NMI(void) {

}

uint8_t cpu::addr_mode_ACCUM(void) {
    return 0;
}

uint8_t cpu::addr_mode_IMM(void) {
    return 0;
}

uint8_t cpu::addr_mode_ABSOLUTE(void) {
    return 0;
}

uint8_t cpu::addr_mode_ZP(void) {
    return 0;
}

uint8_t cpu::addr_mode_ZPX(void) {
    return 0;
}

uint8_t cpu::addr_mode_ZPY(void) {
    return 0;
}

uint8_t cpu::addr_mode_ABSX(void) {
    return 0;
}

uint8_t cpu::addr_mode_ABSY(void) {
    return 0;
}

uint8_t cpu::addr_mode_IMPLIED(void) {
    return 0;
}

uint8_t cpu::addr_mode_RELATIVE(void) {
    return 0;
}

uint8_t cpu::addr_mode_INDIRECT(void) {
    return 0;
}

uint8_t cpu::addr_mode_INDIRECTX(void) {
    return 0;
}

uint8_t cpu::addr_mode_INDIRECTY(void) {
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
    return 0;
}

uint8_t cpu::instr_DEY(void) {
    return 0;
}

uint8_t cpu::instr_EOR(void) {
    return 0;
}

uint8_t cpu::instr_INC(void) {
    return 0;
}

uint8_t cpu::instr_INX(void) {
    return 0;
}

uint8_t cpu::instr_INY(void) {
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
    return 0;
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