#include "cpu.h"

cpu::cpu(bus *bus_ptr, ram *ram_ptr) 
:   bus_device(bus_ptr)
{
    _ram_ptr = ram_ptr; 

    _opcode_decoder_lookup = new opcode[OPCODE_COUNT];      // we need 256 of these, each instruction is used as its index
    init_opcode_decoder_lookup();

    // disable the read and write function pointer
    _read_function_ptr = nullptr;
    _write_function_ptr = nullptr;

    _status_flags_reg.u = 1; // unused is left at one for all time    
 
    reset();
} 

cpu::~cpu() {
    delete[] _opcode_decoder_lookup;
}

void cpu::cycle(void) {
    if (_instr_cycles == 0) {
        // The program counter will point to an address where there is some data stored.
        // read the data at program counter (8 bits)
        // increment the program counter
        // Decode the data retrieved at PGMCounter
        // Run this through our decoded_instruction set
        // address mode will tell you if you need more clock cycles
        // instruction will tell you if you need more clock cycles (Likely just zero or one)
        // add those cycles to the amount of cycles required by that specific instruction in lookup
        // execute the instruction
    }
    _instr_cycles--;
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

    // set the stack pointer back to the end. As the stack pointer moves, it decrements back to STACK_START
    set_stack_pointer(STACK_END);

   _status_flags_reg.i = 1; // interrupt mask is set (IRQ disabled on reset)
   _status_flags_reg.d = 0; // decimal mode is cleared
} 

bool cpu::IRQ(void) {
    if (_status_flags_reg.i == 0) { // this will only be allowed if the interrupt flag is enabled
        // Set the  IRQ flag to 1 to temporarily disable it
        _status_flags_reg.i = 1; 

        // push the program counter high 8 bits first to stack
        // Then the low 8 bits of the program counter
        // Then status register      

        // TODO - need to add a push to stack function, the stack pointer starts at the top end of the page, and decrements down towards bottom

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

status_flags cpu::get_status_flags_struct(void) {
    return _status_flags_reg;
}

uint8_t cpu::get_status_flags(void) {
    uint8_t status_flags;
    status_flags = _status_flags_reg.c; // << CARRY_FLAG;
    status_flags |= _status_flags_reg.z << ZERO_FLAG;
    status_flags |= _status_flags_reg.i << IRQ_FLAG;
    status_flags |= _status_flags_reg.d << DECIMAL_FLAG;
    status_flags |= _status_flags_reg.b << BREAK_FLAG;
    status_flags |= _status_flags_reg.u << UNUSED_FLAG;
    status_flags |= _status_flags_reg.v << OVERFLOW_FLAG;
    status_flags |= _status_flags_reg.n << NEGATIVE_FLAG;    
    return status_flags;
}

void cpu::set_stack_pointer(uint16_t address) {
    if (address >= STACK_START && address <= STACK_END) {
        _stack_pointer = address & 0x00FF; // chop off the high 8 bits
    }
    else {
        _stack_pointer = 0x00;
    }
}

uint16_t cpu::get_last_fetched_address(void) {
    return _fetched_address;
}

uint8_t cpu::get_last_fetched_operand(void) {
    return _fetched_operand;
}

uint8_t cpu::get_last_fetched_opcode(void) {
    return _instr_opcode;
}

uint8_t cpu::get_last_fetched_instr_cycles(void) {
    return _instr_cycles;
}