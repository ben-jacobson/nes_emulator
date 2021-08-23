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

	void enable_file_logging(void);
	void disable_file_logging(void);
	bool file_logging_enabled(void);

	static constexpr uint8_t INSTRUCTION_COUNT = 10;
	std::vector<std::string> _instruction_trace;

private:
	bool start_new_log_file(std::string filename);
	void close_log_file(void);
	void fetch_and_decode_next_instruction(void);
	void update_trace(void);

	bool _file_logging;
	FILE* _file_handle;
	cpu* _cpu_ptr;
	bus* _bus_ptr;

	std::string _last_decoded_instruction;
	uint16_t _update_pc_check, _current_pc;
};
