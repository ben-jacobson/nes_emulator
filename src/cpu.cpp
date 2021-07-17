#include "cpu.h"

cpu::cpu(bus *bus_ptr, ram *ram_ptr) 
:   bus_device(bus_ptr)
{
    _ram_ptr = ram_ptr; 

    // disable the read and write function pointer
    _read_function_ptr = nullptr;
    _write_function_ptr = nullptr;
 
    reset();
} 

void cpu::cycle(void) {
    // fetch data
    // a cycle can have multiple clock cycles, this is affected by how the instructions are morphed by the addressing modes. 
}

void cpu::reset(void) {
    // On the 6502 hardware, during reset time, writing to or from the CPU is prohibited, at this point our hardware doesn't do anything to prevent this, but we may need to in future
    // System initialization usually takes 6 clock cycles, unsure if this is relevant to our high level emulator

    // Set mask interrupt flag

    // Load program counter from memory vector 0xFFFC and 0xFFFD, which is start location for program control
    _bus_ptr->set_address(RESET_VECTOR_HIGH);  
    _program_counter = _bus_ptr->read_data();
    _program_counter = _program_counter << 8; 
    _bus_ptr->set_address(RESET_VECTOR_LOW);
    _program_counter |= _bus_ptr->read_data();

    // set the stack pointer back to the start address
    set_stack_pointer(STACK_START);

    // reset all status flags to default values
   _status_flags_reg.n = 0;
   _status_flags_reg.v = 0;
   _status_flags_reg.u = 0;
   _status_flags_reg.b = 0;
   _status_flags_reg.d = 0;
   _status_flags_reg.i = 0;
   _status_flags_reg.z = 0;
   _status_flags_reg.c = 0;
} 

bool cpu::IRQ(void) {
    if (_status_flags_reg.i == 0) { // this will only be allowed if the interrupt flag is enabled
        // Set the  IRQ flag to 1 to temporarily disable it
        _status_flags_reg.i = 1; 

        // store the program counter and status flags on the stack
        //_program_counter -> stack??
        //_status_flags_reg -> stack??

        // CPU expects that the interrupt vector will be loaded into addresses FFFE and FFFF (within the ROM space)
        _bus_ptr->set_address(0xFFFF);           // set program counter high bits from address FFFF    
        _program_counter = _bus_ptr->read_data();
        _program_counter = _program_counter << 8; 
        _bus_ptr->set_address(0xFFFE); // set program counter low bits from address FFFE
        _program_counter |= _bus_ptr->read_data();             
        return true; 
    }
    return false;
}

bool cpu::NMI(void) {
    // store the program counter and status flags on the stack
    //_program_counter -> stack??
    //_status_flags_reg -> stack??

    // CPU expects that the interrupt vector will be loaded into FFFA and FFFB (within the ROM space)
    _bus_ptr->set_address(0xFFFB);           // set program counter high bits from address FFFB    
    _program_counter = _bus_ptr->read_data();
    _program_counter = _program_counter << 8; 
    _bus_ptr->set_address(0xFFFA); // set program counter low bits from address FFFA
    _program_counter |= _bus_ptr->read_data();             
    return true;
}

uint8_t cpu::read(uint16_t address) {
    return 0; // do nothing
}

void cpu::write(uint16_t address, uint8_t data) {
    return; // do nothing
}	

uint16_t cpu::get_program_counter(void) {
    return _program_counter;
}

uint8_t cpu::get_accumulator_reg_content(void) {
    return _accumulator_reg;
}

uint8_t cpu::get_x_index_reg_content(void) {
    return _x_index_reg;
}

uint8_t cpu::get_y_index_reg_content(void) {
    return _y_index_reg;
}

uint8_t cpu::get_stack_pointer(void) {
    return _stack_pointer;
}

status_flags cpu::get_status_reg_flags_contents(void) {
    return _status_flags_reg;
}

void cpu::set_stack_pointer(uint16_t address) {
    if (address >= STACK_START && address <= STACK_END) {
        _stack_pointer = address & 0x00FF; // chop off the high 8 bits
    }
    else {
        _stack_pointer = 0x00;
    }
}


uint8_t cpu::addr_mode_ACCUM(void) {
    return 0;
}

uint8_t cpu::addr_mode_IMM(void) {
    return 0;
}

uint8_t cpu::addr_mode_ABS(void) {
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

uint8_t cpu::addr_mode_INDI(void) {
    return 0;
}

uint8_t cpu::addr_mode_INDX(void) {
    return 0;
}

uint8_t cpu::addr_mode_INDY(void) {
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

uint8_t cpu::instr_ZZZ(void) {
    return 0;
}