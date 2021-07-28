#include "cpu.h"

uint8_t cpu::instr_ADC(void) {
    _bus_ptr->set_address(_fetched_address);
    uint8_t memory = _bus_ptr->read_data();
    check_if_overflow(_accumulator_reg, memory, _accumulator_reg + memory + _status_flags_reg.c);
    check_if_carry(_accumulator_reg + memory + _status_flags_reg.c);
    _accumulator_reg += memory + _status_flags_reg.c;
    check_if_negative(_accumulator_reg);
    check_if_zero(_accumulator_reg);    
    return 0;
}

uint8_t cpu::instr_AND(void) {
    _bus_ptr->set_address(_fetched_address);
    _accumulator_reg &= _bus_ptr->read_data();
    check_if_negative(_accumulator_reg);
    check_if_zero(_accumulator_reg); 
    return 0;
}

uint8_t cpu::instr_ASL(void) {
    if (_accumulator_addressing_mode) {
        _accumulator_reg = _accumulator_reg << 1;
    }
    else {
        _bus_ptr->set_address(_fetched_address);
        uint8_t data = _bus_ptr->read_data();
        _status_flags_reg.c = check_bit(data, 7); // if we are shifting left, the MSB being a 1 would indicate the overflow. Must be checked before processing the data
        data = data << 1; 
        _bus_ptr->write_data(data);
        check_if_negative(data);
        check_if_zero(data);  
    }

    return 0;
}

uint8_t cpu::instr_BCC(void) {
    if (_status_flags_reg.c == 0) {
        set_program_counter(_fetched_address);

        if ((_fetched_address & 0xFF00) == (get_program_counter() & 0xFF00)) {  // if the branch occurs within the same page, add one more clock cycle
            return 1; 
        }
    }
    return 0;
}

uint8_t cpu::instr_BCS(void) {
    if (_status_flags_reg.c == 1) {
        set_program_counter(_fetched_address);

        if ((_fetched_address & 0xFF00) == (get_program_counter() & 0xFF00)) {  // if the branch occurs within the same page, add one more clock cycle
            return 1; 
        }
    }
    return 0;
}

uint8_t cpu::instr_BEQ(void) {
    if (_status_flags_reg.z == 1) {
        set_program_counter(_fetched_address);

        if ((_fetched_address & 0xFF00) == (get_program_counter() & 0xFF00)) {  // if the branch occurs within the same page, add one more clock cycle
            return 1; 
        }
    }
    return 0;
}

uint8_t cpu::instr_BIT(void) {
    // bits 7 and 6 of operand are transfered to N and V of the status register;
    _bus_ptr->set_address(_fetched_address);
    uint8_t data = _bus_ptr->read_data();

    _status_flags_reg.n = check_bit(data, 7);
    _status_flags_reg.v = check_bit(data, 6);

    // the zero-flag is set to the result of operand AND accumulator.
    check_if_zero(data & _accumulator_reg);
    return 0;   
}

uint8_t cpu::instr_BMI(void) {
    if (_status_flags_reg.n == 1) {
        set_program_counter(_fetched_address);

        if ((_fetched_address & 0xFF00) == (get_program_counter() & 0xFF00)) {  // if the branch occurs within the same page, add one more clock cycle
            return 1; 
        }
    }
    return 0;
}                                          

uint8_t cpu::instr_BNE(void) {
    if (_status_flags_reg.z == 0) {
        set_program_counter(_fetched_address);

        if ((_fetched_address & 0xFF00) == (get_program_counter() & 0xFF00)) {  // if the branch occurs within the same page, add one more clock cycle
            return 1; 
        }
    }
    return 0;
}

uint8_t cpu::instr_BPL(void) {
    if (_status_flags_reg.n == 0) {
        set_program_counter(_fetched_address);

        if ((_fetched_address & 0xFF00) == (get_program_counter() & 0xFF00)) {  // if the branch occurs within the same page, add one more clock cycle
            return 1; 
        }
    }
    return 0;
}

uint8_t cpu::instr_BRK(void) {
    program_counter_to_stack(2); // PC+2 is pushed to stack
    push_to_stack(get_status_flags());
    _status_flags_reg.b = 1; 
    _hit_break = true;
    return 0;
}

uint8_t cpu::instr_BVC(void) {
    if (_status_flags_reg.v == 0) {
        set_program_counter(_fetched_address);

        if ((_fetched_address & 0xFF00) == (get_program_counter() & 0xFF00)) {  // if the branch occurs within the same page, add one more clock cycle
            return 1; 
        }
    }
    return 0;
}

uint8_t cpu::instr_BVS(void) {
    if (_status_flags_reg.v == 1) {
        set_program_counter(_fetched_address);

        if ((_fetched_address & 0xFF00) == (get_program_counter() & 0xFF00)) {  // if the branch occurs within the same page, add one more clock cycle
            return 1; 
        }
    }
    return 0;
}

uint8_t cpu::instr_CLC(void) {
    _status_flags_reg.c = 0;
    return 0;
}                                              
uint8_t cpu::instr_CLD(void) {
    _status_flags_reg.d = _status_flags_reg.d;    // our processor does not use decimal mode, so this has been disabled for nes accuract
    return 0;
}

uint8_t cpu::instr_CLI(void) {
    _status_flags_reg.i = 0;
    return 0;
}

uint8_t cpu::instr_CLV(void) {
    _status_flags_reg.v = 0;
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
    _bus_ptr->set_address(_fetched_address);
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
    _bus_ptr->set_address(_fetched_address);
    _accumulator_reg = _accumulator_reg ^ _bus_ptr->read_data();
    check_if_negative(_accumulator_reg);
    check_if_zero(_accumulator_reg);    
    return 0;
}

uint8_t cpu::instr_INC(void) {
    _bus_ptr->set_address(_fetched_address);
    _bus_ptr->write_data(_bus_ptr->read_data() + 1);
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
    set_program_counter(_fetched_address);
    return 0;
}

uint8_t cpu::instr_JSR(void) {
    program_counter_to_stack();     
    set_program_counter(_fetched_address);  // Jump to new location
    return 0;
}

uint8_t cpu::instr_LDA(void) {
    _bus_ptr->set_address(_fetched_address);
    _accumulator_reg = _bus_ptr->read_data();       
    check_if_negative(_accumulator_reg);
    check_if_zero(_accumulator_reg);      
    return 0;
}

uint8_t cpu::instr_LDX(void) {
    _bus_ptr->set_address(_fetched_address); 
    _x_index_reg = _bus_ptr->read_data();      
    check_if_negative(_x_index_reg);
    check_if_zero(_x_index_reg);  
    return 0;
}

uint8_t cpu::instr_LDY(void) {
    _bus_ptr->set_address(_fetched_address); 
    _y_index_reg = _bus_ptr->read_data();      
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
    program_counter_from_stack();
    return 0;
}

uint8_t cpu::instr_SBC(void) {
    return 0;
}

uint8_t cpu::instr_SEC(void) {
    _status_flags_reg.c = 1;
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
    _bus_ptr->set_address(_fetched_address);
    _bus_ptr->write_data(_accumulator_reg); 
    return 0;
}

uint8_t cpu::instr_STX(void) {
    _bus_ptr->set_address(_fetched_address);
    _bus_ptr->write_data(_x_index_reg);
    return 0;
}

uint8_t cpu::instr_STY(void) {
    _bus_ptr->set_address(_fetched_address);
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