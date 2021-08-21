#include "cpu.h"

uint8_t cpu::instr_ADC(void) {
    _bus_ptr->set_address(_fetched_address);
    uint8_t memory = _bus_ptr->read_data();
    check_if_overflow(_accumulator_reg, memory, _accumulator_reg + memory + _status_flags_reg.c);  

    uint8_t carry = _status_flags_reg.c;    
    check_if_carry(_accumulator_reg + memory + _status_flags_reg.c);
    
    _accumulator_reg += memory + carry; // need to be careful, the previous instruction resets our carry bit, and we need this as part of the instruction
    check_if_negative(_accumulator_reg);
    check_if_zero(_accumulator_reg);    

    /*if (_status_flags_reg.d == 1) { // we add an additional clock cycle if in decimal mode, disabled as our test rom doesn't seem to care. 
        return 1; 
    }*/    
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
        _status_flags_reg.c = check_bit(_accumulator_reg, 7); // if we are shifting left, the MSB being a 1 would indicate the overflow. Must be checked before processing the data
        _accumulator_reg = _accumulator_reg << 1;
        check_if_negative(_accumulator_reg);
        check_if_zero(_accumulator_reg);  
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
    _status_flags_reg.d = 0;    
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
    _bus_ptr->set_address(_fetched_address);
    uint8_t memory = _bus_ptr->read_data();
    //check_if_carry(_accumulator_reg - memory); // our check if carry will not properly set the carry flag on subtraction, so am not using it here
    _status_flags_reg.c = _accumulator_reg >= memory ? 1 : 0;
    check_if_zero(_accumulator_reg - memory);
    check_if_negative(_accumulator_reg - memory);
    //  _accumulator_reg -= memory;    // this caused an interesting few bugs!! I thought the accumulator subtracted it. Nope, the result isn't stored anywhere, this is just for checking flags!
    return 0;
}

uint8_t cpu::instr_CPX(void) {
    _bus_ptr->set_address(_fetched_address);
    uint8_t memory = _bus_ptr->read_data();
    _status_flags_reg.c = _x_index_reg >= memory ? 1 : 0;
    check_if_zero(_x_index_reg - memory);
    check_if_negative(_x_index_reg - memory);
    return 0;
}

uint8_t cpu::instr_CPY(void) {
    _bus_ptr->set_address(_fetched_address);
    uint8_t memory = _bus_ptr->read_data();
    _status_flags_reg.c = _y_index_reg >= memory ? 1 : 0;
    check_if_zero(_y_index_reg - memory);
    check_if_negative(_y_index_reg - memory);    
    return 0;
}

uint8_t cpu::instr_DEC(void) {
    _bus_ptr->set_address(_fetched_address);
    uint8_t memory = _bus_ptr->read_data() - 1;
    check_if_negative(memory);
    check_if_zero(memory);
    _bus_ptr->write_data(memory);    
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
    uint8_t memory = _bus_ptr->read_data() + 1;
    check_if_zero(memory);
    check_if_negative(memory);
    _bus_ptr->write_data(memory);
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
    //std::cout << "JSR instruction. Pushing: 0x" << std::hex << (get_program_counter() - 1) << " to stack" << std::endl;
    program_counter_to_stack(-1); // push PC+2 to stack. -1 offset is because PC will have already progress 3 places by now (JSR is a 3 byte instruction)
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
    if (_accumulator_addressing_mode) {
        _status_flags_reg.c = _accumulator_reg & 1;
        _accumulator_reg = _accumulator_reg >> 1;
        check_if_zero(_accumulator_reg);
    }
    else {
        _bus_ptr->set_address(_fetched_address);
        uint8_t memory = _bus_ptr->read_data();
        _status_flags_reg.c = memory & 1;
        memory = memory >> 1; 
        check_if_zero(memory);
        _bus_ptr->write_data(memory);
    }
    _status_flags_reg.n = 0;
    return 0;
}

uint8_t cpu::instr_NOP(void) {
    // do nothing
    return 0;
}

uint8_t cpu::instr_ORA(void) {
    _bus_ptr->set_address(_fetched_address);
    uint8_t memory = _bus_ptr->read_data(); 
    _accumulator_reg |= memory;
    check_if_negative(_accumulator_reg);
    check_if_zero(_accumulator_reg);
    return 0;
}

uint8_t cpu::instr_PHA(void) {
    push_to_stack(_accumulator_reg);
    return 0;
}

uint8_t cpu::instr_PHP(void) {
    uint8_t status_flags = get_status_flags() | (1 << BREAK_FLAG) | (1 << UNUSED_FLAG);
    push_to_stack(status_flags);
    return 0;   // no more cycles needed
}

uint8_t cpu::instr_PLA(void) {
    _accumulator_reg = pull_from_stack();
    check_if_negative(_accumulator_reg);
    check_if_zero(_accumulator_reg);
    return 0;
}

uint8_t cpu::instr_PLP(void) {
    status_register_from_stack();
    return 0;
}

uint8_t cpu::instr_ROL(void) {
    if (_accumulator_addressing_mode) {
        uint8_t carry_status = _status_flags_reg.c;
        _status_flags_reg.c = check_bit(_accumulator_reg, 7); // if we are shifting left, the MSB being a 1 would indicate the carry. Must be checked before processing the data
        _accumulator_reg = (_accumulator_reg << 1) | carry_status;  // place the carry bit at the LSB position. 
        check_if_negative(_accumulator_reg);
        check_if_zero(_accumulator_reg);        
    }
    else {
        _bus_ptr->set_address(_fetched_address);
        uint8_t memory = _bus_ptr->read_data();
        uint8_t carry_status = _status_flags_reg.c;
        _status_flags_reg.c = check_bit(memory, 7); // if we are shifting left, the MSB being a 1 would indicate the carry. Must be checked before processing the data
        memory = (memory << 1) | carry_status;  // place the carry bit at the LSB position. 
        check_if_negative(memory);
        check_if_zero(memory);
        _bus_ptr->write_data(memory);
    }
    return 0;
}

uint8_t cpu::instr_ROR(void) {
    if (_accumulator_addressing_mode) {
        uint8_t carry = _accumulator_reg & 1;
        _accumulator_reg = (_accumulator_reg >> 1) | (_status_flags_reg.c << 7);  
        _status_flags_reg.c = carry;     
        check_if_negative(_accumulator_reg);
        check_if_zero(_accumulator_reg);
    }
    else {
        _bus_ptr->set_address(_fetched_address);
        uint8_t memory = _bus_ptr->read_data();
        uint8_t carry = memory & 1;
        memory = (memory >> 1) | (_status_flags_reg.c << 7);  // place the carry bit at the MSB position if it's set. 
        _status_flags_reg.c = carry;
        check_if_negative(memory);
        check_if_zero(memory);
        _bus_ptr->write_data(memory);
    }
    return 0;
}

uint8_t cpu::instr_RTI(void) {
    status_register_from_stack();  
    program_counter_from_stack();
    return 0;
}

uint8_t cpu::instr_RTS(void) {
    program_counter_from_stack(1);
    return 0;
}

uint8_t cpu::instr_SBC(void) {
    _bus_ptr->set_address(_fetched_address);
    uint8_t memory = _bus_ptr->read_data();
    uint8_t borrow = (1 - _status_flags_reg.c); // important that we set the inverse of carry as the borrow before checking the carry, as it would reset the carry before we needed to use it.
    check_if_overflow(_accumulator_reg, -memory, _accumulator_reg - memory - borrow);  
    //check_if_carry(_accumulator_reg - memory - borrow);    // our check if carry will not properly set the carry flag on subtraction, so am not using it here
    _status_flags_reg.c = _accumulator_reg >= memory + borrow ? 1 : 0;

    _accumulator_reg = _accumulator_reg - memory - borrow;
    check_if_negative(_accumulator_reg);
    check_if_zero(_accumulator_reg);

    /*if (_status_flags_reg.d == 1) { // we add an additional clock cycle if in decimal mode, disabled as our test rom doesn't seem to care. 
        return 1; 
    }*/
    return 0;
}

uint8_t cpu::instr_SEC(void) {
    _status_flags_reg.c = 1;     
    return 0;
}

uint8_t cpu::instr_SED(void) {
    _status_flags_reg.d = 1;
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
    _x_index_reg = _accumulator_reg;
    check_if_negative(_x_index_reg);
    check_if_zero(_x_index_reg);
    return 0;
}

uint8_t cpu::instr_TAY(void) {
    _y_index_reg = _accumulator_reg;
    check_if_negative(_y_index_reg);
    check_if_zero(_y_index_reg);    
    return 0;
}

uint8_t cpu::instr_TSX(void) {
    _x_index_reg = _stack_pointer;
    check_if_negative(_x_index_reg);
    check_if_zero(_x_index_reg);    
    return 0;
}

uint8_t cpu::instr_TXA(void) {
    _accumulator_reg = _x_index_reg;
    check_if_negative(_accumulator_reg);
    check_if_zero(_accumulator_reg);
    return 0;
}

uint8_t cpu::instr_TXS(void) {
    _stack_pointer = _x_index_reg;
    return 0;
}

uint8_t cpu::instr_TYA(void) {
    _accumulator_reg = _y_index_reg;
    check_if_negative(_accumulator_reg);
    check_if_zero(_accumulator_reg);
    return 0;
}

uint8_t cpu::instr_ZZZ(void) {
    // do nothing
    return 0;
}