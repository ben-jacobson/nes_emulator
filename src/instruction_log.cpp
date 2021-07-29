#include "instruction_log.h"

instruction_log::instruction_log(std::string log_filename, cpu* cpu_ptr, bus* bus_ptr) {
    start_new_log_file(log_filename);
    _cpu_ptr = cpu_ptr;
}
	
instruction_log::~instruction_log() {
    close_log_file();
}

void instruction_log::update_instruction_log(void) {

}

bool instruction_log::start_new_log_file(std::string filename) {
    _file_handle = fopen(filename.c_str(), "w");  

    if (_file_handle == NULL) {
        return false; 
    }

    rewind(_file_handle);
    fwrite("asdf", 1, 4, _file_handle);

    return true; 
}

void instruction_log::close_log_file() {
    fclose(_file_handle);
}