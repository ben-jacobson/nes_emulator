#pragma once

#include "cpu.h"

class instruction_log
{
public:
	instruction_log(std::string log_filename, cpu* cpu_ptr, bus* bus_ptr);
	~instruction_log();
	void update_instruction_log(void);

private:
	bool start_new_log_file(std::string filename);
	void close_log_file();

	static constexpr uint8_t INSTRUCTION_COUNT = 10;
	cpu* _cpu_ptr;
	FILE* _file_handle;
};
