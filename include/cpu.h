#pragma once

#include "memory_map.h"
#include "bus_device.h"
#include "bus.h"
#include "ram.h"

// good reading for this: http://archive.6502.org/datasheets/rockwell_r650x_r651x.pdf

struct status_flags {
    uint8_t n : 1;          // negative flag, 1 = negative.
    uint8_t v : 1;          // overflow flag. 
    uint8_t u : 1;          // unused, should be 1 at all times.
    uint8_t b : 1;          // break command flag, 1 = break.
    uint8_t d : 1;          // decimal mode.   
    uint8_t i : 1;          // IRQ disable flag, 1 = disable.
    uint8_t z : 1;          // zero flag, 1 = result zero.
    uint8_t c : 1;          // carry flag.
};

class cpu : public bus_device
{
public:
	cpu(bus *bus_ptr, ram *ram_ptr);
	~cpu() = default;

    // CPU main functions
    void cycle(void);   // main CPU clocking, called cycle because a cycle can have multiple clock pulses, e.g in multi clock instructions
    void reset(void);   // reset CPU back to original state. 
    void IRQ(void);     // Interrupt request
    void NMI(void);     // non maskable interrupt
    uint8_t fetch_data(void);    

    // being a bus device, we need to define these, as a result, they do nothing. 
	uint8_t read(uint16_t address) override;
	void write(uint16_t address, uint8_t data) override;	   

    uint16_t get_program_counter(void);
    uint8_t get_accumulator_reg_content(void);
    uint8_t get_x_index_reg_content(void);
    uint8_t get_y_index_reg_content(void);
    status_flags get_status_reg_flags_contents(void);
    uint8_t get_stack_pointer(void); 

private:
    ram* _ram_ptr;  // store a pointer to ram so that we can send write commands to it, sorta like how the CPU controls the chip enables

    uint16_t program_counter; // program counter increments each time an instruction or data is fetched from memory. 
    uint8_t accumulator_reg; 
    uint8_t x_index_reg, y_index_reg;     
    uint8_t stack_pointer;      
    status_flags status_flags_reg; 

    // Functions are for setting the addressing modes
    uint8_t addr_mode_ACCUM(void);
    uint8_t addr_mode_IMM(void);
    uint8_t addr_mode_ABS(void);
    uint8_t addr_mode_ZP(void);
    uint8_t addr_mode_ZPX(void);
    uint8_t addr_mode_ZPY(void);
    uint8_t addr_mode_ABSX(void);
    uint8_t addr_mode_ABSY(void);
    uint8_t addr_mode_IMPLIED(void);
    uint8_t addr_mode_RELATIVE(void);
    uint8_t addr_mode_INDI(void);
    uint8_t addr_mode_INDX(void);
    uint8_t addr_mode_INDY(void);

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
};
