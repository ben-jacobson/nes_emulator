#pragma once

#include <iomanip>
#include <vector>
#include <string>
#include "cpu.h"

class instruction_log
{
public:
	instruction_log(std::string log_filename, cpu* cpu_ptr, bus* bus_ptr);
	~instruction_log();
	void update(void);

private:
	bool start_new_log_file(std::string filename);
	void close_log_file(void);
	void fetch_and_decode_next_instruction(void);
	void update_trace(void);

	FILE* _file_handle;
	cpu* _cpu_ptr;
	bus* _bus_ptr;

	static constexpr uint8_t INSTRUCTION_COUNT = 10;
	std::vector<std::string> _instruction_trace;
	std::string _last_decoded_instruction;
	uint16_t _current_pc;
};
