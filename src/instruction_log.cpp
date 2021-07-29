#include "instruction_log.h"

instruction_log::instruction_log(std::string log_filename, cpu* cpu_ptr, bus* bus_ptr) {
    start_new_log_file(log_filename);
    _cpu_ptr = cpu_ptr;
    _bus_ptr = bus_ptr;

    _current_pc = 0; // we want the first update to write straight away. 

    _instruction_trace.reserve(INSTRUCTION_COUNT + 1);

    // fill the instruction trace with dummy data for now
    for (uint8_t i = 0; i < INSTRUCTION_COUNT; i++) {
        std::cout << (uint16_t)i << std::endl;
        _instruction_trace[i] = "NULL";
    }    

    std::cout << "here2" << std::endl;

}
	
instruction_log::~instruction_log() {
    close_log_file();
}

bool instruction_log::start_new_log_file(std::string filename) {
    _file_handle = fopen(filename.c_str(), "w");  

    if (_file_handle == NULL) {
        return false; 
    }

    rewind(_file_handle);
    return true; 
}

void instruction_log::close_log_file(void) {
    fclose(_file_handle);
}

void instruction_log::update(void) {
    // only update if we've moved to the next address
    if (_current_pc != _cpu_ptr->get_program_counter()) {
        // update the address, decode it and finally write it into the file
        _current_pc = _cpu_ptr->get_program_counter();
        fetch_and_decode_next_instruction();
        fwrite(_last_decoded_instruction.c_str(), 1, _last_decoded_instruction.length(), _file_handle);

        // update the array of strings showing the previous 3 instructions and the next 7 (depending on what is set with the constant INSTRUCTION_COUNT)
        update_trace();
    }
}

void instruction_log::update_trace(void) {
    // we can't just update the next element in the trace, we need to actually rebuild it every time, just in case we've used a jump instruction
    // this could be optimized, but it doesn't matter a great deal seeing as this is just for debugging display which is optional for the final user experence
    uint8_t current_instruction_pos = 3; 

    //_instruction_trace.erase(_instruction_trace.begin()); // remove the first element in vector and restack the trace
    //_instruction_trace[current_instruction_pos] = _last_decoded_instruction;  // the first three lines are previous instructions, the fourth line is the current instruction. 

/*    for (uint8_t i = current_instruction_pos + 1; i < _instruction_trace.size(); i++) {
        fetch_and_decode_next_instruction();
        _instruction_trace[i] = _last_decoded_instruction;
    }

    std::cout << std::endl << std::endl << "Instruction Trace: " << std::endl;
    for (uint8_t i = 0; i < _instruction_trace.size(); i++) {
        std::cout << _instruction_trace[i] << std::endl;
    }*/
}

void instruction_log::fetch_and_decode_next_instruction(void) {
    _bus_ptr->set_address(_current_pc);
    uint8_t opcode = _bus_ptr->read_data(); // we are already at the address

    std::string instruction_name = _cpu_ptr->_opcode_decoder_lookup[opcode].name;
    std::string address_mode = _cpu_ptr->_opcode_decoder_lookup[opcode].address_mode_name;
    uint8_t instruction_bytes = _cpu_ptr->_opcode_decoder_lookup[opcode].instruction_bytes;
    uint8_t operand; 

    std::stringstream decoded_line;
    decoded_line << "0x" << std::setfill('0') << std::setw(4) << std::uppercase << std::hex << _current_pc << ": ";
    decoded_line << "0x" << std::setfill('0') << std::setw(2) << std::uppercase << std::hex << (uint16_t)opcode << " " << instruction_name << " ";

    if (instruction_bytes > 1) {
        for (uint8_t i = 0; i < instruction_bytes - 1; i++) {   // print as many operands as needed
            _current_pc++;
            _bus_ptr->set_address(_current_pc);
            operand = _bus_ptr->read_data();
            decoded_line << "0x" << std::setfill('0') << std::setw(2) << std::uppercase << std::hex << (uint16_t)operand << " ";
        }
    }

    decoded_line << "[" << address_mode << "]" << "\n"; 
    _last_decoded_instruction = decoded_line.str();
    _current_pc++;
}