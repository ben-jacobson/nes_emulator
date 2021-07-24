#pragma once

#include "memory_map.h"
#include "bus_device.h"
#include "bus.h"
#include "ram.h"

#include <functional>

// good reading for this: http://archive.6502.org/datasheets/rockwell_r650x_r651x.pdf
// Also good: http://users.telenet.be/kim1-6502/6502/proman.html

constexpr uint8_t OPCODE_COUNT = 255;
constexpr uint8_t RESET_CYCLES = 7; // 7 clock cycles to reset CPU, the datasheet says 6, but our test rom says 7.

enum status_flags_map : uint8_t {
    CARRY_FLAG      = 0,          // carry flag         
    ZERO_FLAG       = 1,          // zero flag
    IRQ_FLAG        = 2,          // IRQ disable flag
    DECIMAL_FLAG    = 3,          // decimal mode  
    BREAK_FLAG      = 4,          // break command flag
    UNUSED_FLAG     = 5,          // unused, should be 1 at all times.
    OVERFLOW_FLAG   = 6,          // overflow flag 
    NEGATIVE_FLAG   = 7,          // negative flag
};

struct status_flags {
    uint8_t c : 1;          // carry flag.
    uint8_t z : 1;          // zero flag, 1 = result zero.
    uint8_t i : 1;          // IRQ disable flag, 1 = disable.
    uint8_t d : 1;          // decimal mode.   
    uint8_t b : 1;          // break command flag, 1 = break.
    uint8_t u : 1;          // unused, should be 1 at all times.
    uint8_t v : 1;          // overflow flag. 
    uint8_t n : 1;          // negative flag, 1 = negative.
};

struct opcode {
    std::string name;
    std::function<uint8_t(void)> instruction;
    std::function<uint8_t(void)> address_mode;
    std::string address_mode_name;
    uint8_t instruction_bytes;
    uint8_t cycles_needed;
};

class cpu : public bus_device
{
public:
    // constrcutor and destructor
	cpu(bus *bus_ptr);
	~cpu();

    uint16_t _cycle_count;

    // opcode decoder struct
    opcode* _opcode_decoder_lookup;

    // CPU main functions
    void cycle(void);   // main CPU clocking, called cycle because a cycle can have multiple clock pulses, e.g in multi clock instructions
    void reset(void);   // reset CPU back to original state. 
    bool IRQ(void);     // Interrupt request
    bool NMI(void);     // non maskable interrupt
    //void fetch_opcode(void);    

    uint16_t debug_get_cycle_count(void);
    void debug_set_x_register(uint8_t data);
    void debug_set_y_register(uint8_t data);
    void debug_set_acc_register(uint8_t data);

    // being a bus device, we need to define these, as a result, they do nothing. 
	uint8_t read(uint16_t address) override;
	void write(uint16_t address, uint8_t data) override;	   

    void set_program_counter(uint16_t new_program_counter);
    uint16_t get_program_counter(void);

    uint8_t get_accumulator_reg_content(void);
    uint8_t get_x_index_reg_content(void);
    uint8_t get_y_index_reg_content(void);
    uint8_t get_status_flags(void);
    status_flags get_status_flags_struct(void);
    bool finished_instruction(void);

    uint16_t get_last_fetched(void);
    uint8_t get_last_fetched_opcode(void);
    uint8_t get_last_fetched_instr_cycles(void);

    void set_stack_pointer(uint16_t offset_address);
    uint8_t get_stack_pointer(void); 

    // Functions are for setting the addressing modes, in alphabetical order
    uint8_t addr_mode_ABS(void);
    uint8_t addr_mode_ABSX(void);
    uint8_t addr_mode_ABSY(void);
    uint8_t addr_mode_ACC(void);
    uint8_t addr_mode_IMM(void);
    uint8_t addr_mode_IMP(void);
    uint8_t addr_mode_INDI(void);
    uint8_t addr_mode_INDX(void);
    uint8_t addr_mode_INDY(void);
    uint8_t addr_mode_REL(void);
    uint8_t addr_mode_ZP(void);
    uint8_t addr_mode_ZPX(void);
    uint8_t addr_mode_ZPY(void);

    // Instruction set helper functions
    inline void check_if_negative(uint8_t data) {
        _status_flags_reg.n = ((data & 0x80) >> 7) == 1 ? 1 : 0;
    }

    inline void check_if_zero(uint8_t data) {
        _status_flags_reg.z = data == 0 ? 1: 0;
    } 
    
    // Instruction set in alphabetical order
    uint8_t instr_ADC(void);
    uint8_t instr_AND(void);
    uint8_t instr_ASL(void);

    uint8_t instr_BCC(void);
    uint8_t instr_BCS(void);
    uint8_t instr_BEQ(void);
    uint8_t instr_BIT(void);
    uint8_t instr_BMI(void);                                          
    uint8_t instr_BNE(void);
    uint8_t instr_BPL(void);
    uint8_t instr_BRK(void);
    uint8_t instr_BVC(void);
    uint8_t instr_BVS(void);

    uint8_t instr_CLC(void);                                              
    uint8_t instr_CLD(void);
    uint8_t instr_CLI(void);
    uint8_t instr_CLV(void);
    uint8_t instr_CMP(void);
    uint8_t instr_CPX(void);
    uint8_t instr_CPY(void);

    uint8_t instr_DEC(void);
    uint8_t instr_DEX(void);
    uint8_t instr_DEY(void);

    uint8_t instr_EOR(void);

    uint8_t instr_INC(void);
    uint8_t instr_INX(void);
    uint8_t instr_INY(void);

    uint8_t instr_JMP(void);
    uint8_t instr_JSR(void);

    uint8_t instr_LDA(void);
    uint8_t instr_LDX(void);
    uint8_t instr_LDY(void);
    uint8_t instr_LSR(void);

    uint8_t instr_NOP(void);

    uint8_t instr_ORA(void);

    uint8_t instr_PHA(void);
    uint8_t instr_PHP(void);
    uint8_t instr_PLA(void);
    uint8_t instr_PLP(void);

    uint8_t instr_ROL(void);
    uint8_t instr_ROR(void);
    uint8_t instr_RTI(void);
    uint8_t instr_RTS(void);

    uint8_t instr_SBC(void);
    uint8_t instr_SEC(void);
    uint8_t instr_SED(void);
    uint8_t instr_SEI(void);
    uint8_t instr_STA(void);
    uint8_t instr_STX(void);
    uint8_t instr_STY(void);

    uint8_t instr_TAX(void);
    uint8_t instr_TAY(void);
    uint8_t instr_TSX(void);
    uint8_t instr_TXA(void);
    uint8_t instr_TXS(void);
    uint8_t instr_TYA(void);

    uint8_t instr_ZZZ(void); // for any illegal opcodes

private:
    void init_opcode_decoder_lookup(void);
    void set_opcode(uint8_t index, std::function<uint8_t(void)> instruction, std::string name, std::function<uint8_t(void)> address_mode, std::string address_mode_name, uint8_t instruction_bytes, uint8_t cycles_needed);

    void program_counter_to_stack(void);
    void program_counter_to_stack(uint8_t offset);
    void push_to_stack(uint8_t data);

    // variables used for processing information, passing data between fetch, clock and whatever instruction being performed.
    uint16_t _fetched; // used for whatever fetched address, data or operand from the instruction. We need to be vague because the different address modes use this to prepare data for instructions
    uint8_t _instr_opcode, _instr_cycles;

    uint16_t _program_counter; // program counter increments each time an instruction or data is fetched from memory. 
    uint8_t _accumulator_reg; 
    uint8_t _x_index_reg, _y_index_reg;     
    uint8_t _stack_pointer;      
    status_flags _status_flags_reg;     
};
