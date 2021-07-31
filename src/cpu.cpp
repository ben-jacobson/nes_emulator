#include "cpu.h"

cpu::cpu(bus *bus_ptr) 
:   bus_device(bus_ptr)
{
    _opcode_decoder_lookup = new opcode[OPCODE_COUNT];      // we need 256 of these, each instruction is used as its index
    init_opcode_decoder_lookup();

    // disable the read and write function pointers
    _read_function_ptr = nullptr; 
    _write_function_ptr = nullptr;

    _accumulator_addressing_mode = false; 
    //  reset(); // don't reset the cpu in the contructor, the user may not have had the chance to register the devices on the bus, so this may not work properly and may even thrown an exception.
} 

cpu::~cpu() {
    delete[] _opcode_decoder_lookup;
}

void cpu::cycle(void) {
    if (_instr_cycles == 0) {
        // The program counter will point to an address where there is some data stored.
        _bus_ptr->set_address(_program_counter);

        // read the data at program counter (8 bits)
        _instr_opcode = _bus_ptr->read_data();
        
        // Decode the data retrieved at PGMCounter, then run this through our decoded addres mode
        _instr_cycles = _opcode_decoder_lookup[_instr_opcode].cycles_needed; 
        _program_counter++; // the address mode function will need to read from the PC location after the opcode
        _instr_cycles += _opcode_decoder_lookup[_instr_opcode].address_mode();     // call the address mode, it will tell you if you need more clock cycles        
        _instr_cycles += _opcode_decoder_lookup[_instr_opcode].instruction();      // call the instruction function, it will tell you if you need more clock cycles (Likely just zero or one)
        _accumulator_addressing_mode = false; // this flag is used to morph instruction behaviour of accumulator addressing mode. 
    }
    _instr_cycles--;
    _cycle_count++;
}

bool cpu::finished_instruction(void) {
    if (_instr_cycles < 1) {
        return true;
    }
    return false;
}

uint16_t cpu::debug_get_cycle_count(void) {
    return _cycle_count;
}

void cpu::debug_set_x_register(uint8_t data) {
    _x_index_reg = data;
}

void cpu::debug_set_y_register(uint8_t data) {
    _y_index_reg = data;
}

void cpu::debug_set_acc_register(uint8_t data) {
    _accumulator_reg = data;
}

uint8_t cpu::debug_env_overflower_test(uint8_t data, int adder) {
    // our code base relies heavily on the uint8_t's and uint16_t's overflowing and underflowing as you'd expect. 
    // used in unit testing to make sure that the development environment is set up correctly and overflows/underflows as expected so that the rest of the code can be trusted to work
    uint8_t result = data + adder;
    return result; 
}


void cpu::reset(void) {
    // WARNING - Do not call this function before registering devices on the bus, it will not function properly and may throw an exception

    // On the 6502 hardware, during reset time, writing to or from the CPU is prohibited, at this point our emulation doesn't do anything to prevent this, but we may need to in future
    // System initialization usually takes a number of clock cycles, unsure if this is relevant to our high level emulator but we'll implement it anyway

    _instr_cycles = RESET_CYCLES;
    // Set program counter from memory loaded into vector 0xFFFC and 0xFFFD, which is start location for program control
    _bus_ptr->set_address(RESET_VECTOR_HIGH);  
    _program_counter = _bus_ptr->read_data();
    _program_counter = _program_counter << 8; 
    _bus_ptr->set_address(RESET_VECTOR_LOW);
    _program_counter |= _bus_ptr->read_data();

    // set the stack pointer back to the end. As the stack pointer moves, it decrements back to STACK_START
    set_stack_pointer(STACK_END);

    _status_flags_reg.c = 0;    
    _status_flags_reg.z = 0;    
    _status_flags_reg.i = 1;    // interrupt mask is set (IRQ disabled on reset)
    _status_flags_reg.d = 0;   
    _status_flags_reg.b = 0;    
    _status_flags_reg.u = 0;    
    _status_flags_reg.v = 0;  
    _status_flags_reg.n = 0;  

    _hit_break = false;

   // clear the accumulator, x_index and y_index registers
   _accumulator_reg = 0;
   _x_index_reg = 0;
   _y_index_reg = 0;

   _cycle_count = 0;

    // we also want the program counter and the status register on the stack
    program_counter_to_stack(); // put the program counter on the stack, high 8 bits first, then low 8 bits.
    instr_PHP();    // Then put the status register on the stack         
} 

bool cpu::IRQ(void) {
    if (_status_flags_reg.i == 0) { // this will only be allowed if the interrupt flag is enabled
        // Set the  IRQ flag to 1 to temporarily disable it
        _status_flags_reg.i = 1; 

        program_counter_to_stack(); // put the program counter on the stack, high 8 bits first, then low 8 bits.
        instr_PHP();    // Then put the status register on the stack      

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
    program_counter_to_stack(); // put the program counter on the stack, high 8 bits first, then low 8 bits. 
    instr_PHP();    // Then put the status register on the stack     

    // CPU expects that the interrupt vector will be loaded into FFFA and FFFB (within the ROM space)
    _bus_ptr->set_address(0xFFFB);           // set program counter high bits from address FFFB    
    _program_counter = _bus_ptr->read_data();
    _program_counter = _program_counter << 8; 
    _bus_ptr->set_address(0xFFFA); // set program counter low bits from address FFFA
    _program_counter |= _bus_ptr->read_data();             
    return true;
}

uint8_t cpu::read(uint16_t address) {
    return address; // do nothing and surpress warning
}

void cpu::write(uint16_t address, uint8_t data) {
    address += data; // to supress warning
    return; // do nothing
}	

void cpu::set_program_counter(uint16_t new_program_counter) {
    _program_counter = new_program_counter;
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

void cpu::program_counter_to_stack(int offset) {
    push_to_stack(((_program_counter + offset) & 0xFF00) >> 8);   // put the high 8 bits on the stack at the current pointer
    push_to_stack((_program_counter + offset) & 0x00FF);   // then put the low 8 bits on the stack at the current pointer    
}

void cpu::program_counter_from_stack(int offset) {
    uint8_t low = pull_from_stack();    // pull in the opposite order from pushing
    uint8_t high = pull_from_stack();
    _program_counter = (high << 8 | low) + offset;
}

void cpu::status_register_from_stack(void) {
    uint8_t new_processor_status = pull_from_stack();
    _status_flags_reg.c = check_bit(new_processor_status, CARRY_FLAG);
    _status_flags_reg.z = check_bit(new_processor_status, ZERO_FLAG);
    _status_flags_reg.i = check_bit(new_processor_status, IRQ_FLAG);
    _status_flags_reg.d = check_bit(new_processor_status, DECIMAL_FLAG);
    _status_flags_reg.v = check_bit(new_processor_status, OVERFLOW_FLAG); // break flag and unused are ignored
    _status_flags_reg.n = check_bit(new_processor_status, NEGATIVE_FLAG);    
}

void cpu::push_to_stack(uint8_t data) {
    _bus_ptr->set_address(STACK_START + _stack_pointer);
    _bus_ptr->write_data(data);
    _stack_pointer--; 
}

uint8_t cpu::pull_from_stack(void) {
    _stack_pointer++;  
    _bus_ptr->set_address(STACK_START + _stack_pointer);
    return _bus_ptr->read_data();
}

uint16_t cpu::get_last_fetched(void) {
    return _fetched_address;
}

uint8_t cpu::get_last_fetched_opcode(void) {
    return _instr_opcode;
}

uint8_t cpu::get_last_fetched_instr_cycles(void) {
    return _instr_cycles;
}