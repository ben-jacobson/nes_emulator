#include "cpu.h"

// a huge thank you to https://github.com/kpmiller for his emulator101 repo, containing an easy to copy matrix of opcodes

#define instr(fn)   std::bind(&cpu::fn, this)
#define addr(fn)    std::bind(&cpu::fn, this)

void cpu::init_opcode_decoder_lookup(void) {
    // start by filling out the illegal opcodes
    for (uint8_t i = 0; i < OPCODE_COUNT; i++) {
        set_opcode(i,    "ZZZ", instr(       instr_ZZZ       ), addr(        addr_mode_IMP       ),     0,     2); 
    }

    // then we'll build out the ones we want

    // Row 0
    set_opcode(0X00,     "BRK",     instr(instr_BRK),    addr(addr_mode_IMP),     1,    7);
    set_opcode(0X01,     "ORA",     instr(instr_ORA),    addr(addr_mode_INDX),     2,    6);
    set_opcode(0X05,     "ORA",     instr(instr_ORA),    addr(addr_mode_ZP),     2,    3);
    set_opcode(0X06,     "ASL",     instr(instr_ASL),    addr(addr_mode_ZP),     2,    5);
    set_opcode(0X08,     "PHP",     instr(instr_PHP),    addr(addr_mode_IMP),     1,    3);
    set_opcode(0X09,     "ORA",     instr(instr_ORA),    addr(addr_mode_IMM),     2,    2);
    set_opcode(0X0A,     "ASL",     instr(instr_ASL),    addr(addr_mode_ACCUM),     1,    2);
    set_opcode(0X0D,     "ORA",     instr(instr_ORA),    addr(addr_mode_ABS),     3,    4);
    set_opcode(0X0E,     "ASL",     instr(instr_ASL),    addr(addr_mode_ABS),     3,    6);    
    
    // Row 1    
    set_opcode(0X10,     "BPL",     instr(instr_BPL),    addr(addr_mode_REL),     2,    2);
    set_opcode(0X11,     "ORA",     instr(instr_ORA),    addr(addr_mode_INDY),     2,    5);
    set_opcode(0X15,     "ORA",     instr(instr_ORA),    addr(addr_mode_ZPX),     2,    4);
    set_opcode(0X16,     "ASL",     instr(instr_ASL),    addr(addr_mode_ZPX),     2,    6);
    set_opcode(0X18,     "CLC",     instr(instr_CLC),    addr(addr_mode_IMP),     1,    2);
    set_opcode(0X19,     "ORA",     instr(instr_ORA),    addr(addr_mode_ABSY),     3,    4);
    set_opcode(0X1D,     "ORA",     instr(instr_ORA),    addr(addr_mode_ABSX),     3,    4);
    set_opcode(0X1E,     "ASL",     instr(instr_ASL),    addr(addr_mode_ABSX),     3,    7);
    
    // Row 2    
    set_opcode(0X20,     "JSR",     instr(instr_JSR),    addr(addr_mode_ABS),     3,    6);
    set_opcode(0X21,     "AND",     instr(instr_AND),    addr(addr_mode_INDX),     2,    6);
    set_opcode(0X24,     "BIT",     instr(instr_BIT),    addr(addr_mode_ZP),     2,    3);
    set_opcode(0X25,     "AND",     instr(instr_AND),    addr(addr_mode_ZP),     2,    3);
    set_opcode(0X26,     "ROL",     instr(instr_ROL),    addr(addr_mode_ZP),     2,    5);
    set_opcode(0X28,     "PLP",     instr(instr_PLP),    addr(addr_mode_IMP),     1,    4);
    set_opcode(0X29,     "AND",     instr(instr_AND),    addr(addr_mode_IMM),     2,    2);
    set_opcode(0X2A,     "ROL",     instr(instr_ROL),    addr(addr_mode_ACCUM),     1,    2);
    set_opcode(0X2C,     "BIT",     instr(instr_BIT),    addr(addr_mode_ABS),     3,    4);
    set_opcode(0X2D,     "AND",     instr(instr_AND),    addr(addr_mode_ABS),     3,    4);
    set_opcode(0X2E,     "ROL",     instr(instr_ROL),    addr(addr_mode_ABS),     3,    6);
    
    // Row 3    
    set_opcode(0X30,     "BMI",     instr(instr_BMI),    addr(addr_mode_REL),     2,    2);
    set_opcode(0X31,     "AND",     instr(instr_AND),    addr(addr_mode_INDY),     2,    5);
    set_opcode(0X35,     "AND",     instr(instr_AND),    addr(addr_mode_ZPX),     2,    4);
    set_opcode(0X36,     "ROL",     instr(instr_ROL),    addr(addr_mode_ZPX),     2,    6);
    set_opcode(0X38,     "SEC",     instr(instr_SEC),    addr(addr_mode_IMP),     1,    2);
    set_opcode(0X39,     "AND",     instr(instr_AND),    addr(addr_mode_ABSY),     3,    4);
    set_opcode(0X3D,     "AND",     instr(instr_AND),    addr(addr_mode_ABSX),     3,    4);
    set_opcode(0X3E,     "ROL",     instr(instr_ROL),    addr(addr_mode_ABSX),     3,    7);
    
    // Row 4    
    set_opcode(0X40,     "RTI",     instr(instr_RTI),    addr(addr_mode_IMP),     1,    6);
    set_opcode(0X41,     "EOR",     instr(instr_EOR),    addr(addr_mode_INDX),     2,    6);
    set_opcode(0X45,     "EOR",     instr(instr_EOR),    addr(addr_mode_ZP),     2,    3);
    set_opcode(0X46,     "LSR",     instr(instr_LSR),    addr(addr_mode_ZP),     2,    5);
    set_opcode(0X48,     "PHA",     instr(instr_PHA),    addr(addr_mode_IMP),     1,    3);
    set_opcode(0X49,     "EOR",     instr(instr_EOR),    addr(addr_mode_IMM),     2,    2);
    set_opcode(0X4A,     "LSR",     instr(instr_LSR),    addr(addr_mode_ACCUM),     1,    2);
    set_opcode(0X4C,     "JMP",     instr(instr_JMP),    addr(addr_mode_ABS),     3,    3);
    set_opcode(0X4D,     "EOR",     instr(instr_EOR),    addr(addr_mode_ABS),     3,    4);
    set_opcode(0X4E,     "LSR",     instr(instr_LSR),    addr(addr_mode_ABS),     3,    6);
    
    // Row 5    
    set_opcode(0X50,     "BVC",     instr(instr_BVC),    addr(addr_mode_REL),     2,    2);
    set_opcode(0X51,     "EOR",     instr(instr_EOR),    addr(addr_mode_INDY),     2,    5);
    set_opcode(0X55,     "EOR",     instr(instr_EOR),    addr(addr_mode_ZPX),     2,    4);
    set_opcode(0X56,     "LSR",     instr(instr_LSR),    addr(addr_mode_ZPX),     2,    6);
    set_opcode(0X58,     "CLI",     instr(instr_CLI),    addr(addr_mode_IMP),     1,    2);
    set_opcode(0X59,     "EOR",     instr(instr_EOR),    addr(addr_mode_ABSY),     3,    4);
    set_opcode(0X5D,     "EOR",     instr(instr_EOR),    addr(addr_mode_ABSX),     3,    4);
    set_opcode(0X5E,     "LSR",     instr(instr_LSR),    addr(addr_mode_ABSX),     3,    7);
    
    // Row 6    
    set_opcode(0X60,     "RTS",     instr(instr_RTS),    addr(addr_mode_IMP),     1,    6);
    set_opcode(0X61,     "ADC",     instr(instr_ADC),    addr(addr_mode_INDX),     2,    6);
    set_opcode(0X65,     "ADC",     instr(instr_ADC),    addr(addr_mode_ZP),     2,    3);
    set_opcode(0X66,     "ROR",     instr(instr_ROR),    addr(addr_mode_ZP),     2,    5);
    set_opcode(0X68,     "PLA",     instr(instr_PLA),    addr(addr_mode_IMP),     1,    4);
    set_opcode(0X69,     "ADC",     instr(instr_ADC),    addr(addr_mode_IMM),     2,    2);
    set_opcode(0X6A,     "ROR",     instr(instr_ROR),    addr(addr_mode_ACCUM),     1,    2);
    set_opcode(0X6C,     "JMP",     instr(instr_JMP),    addr(addr_mode_INDI),     3,    5);
    set_opcode(0X6D,     "ADC",     instr(instr_ADC),    addr(addr_mode_ABS),     3,    4);
    set_opcode(0X6E,     "ROR",     instr(instr_ROR),    addr(addr_mode_ABSX),     3,    7);
    
    // Row 7    
    set_opcode(0X70,     "BVS",     instr(instr_BVS),    addr(addr_mode_REL),     2,    2);
    set_opcode(0X71,     "ADC",     instr(instr_ADC),    addr(addr_mode_INDY),     2,    5);
    set_opcode(0X75,     "ADC",     instr(instr_ADC),    addr(addr_mode_ZPX),     2,    4);
    set_opcode(0X76,     "ROR",     instr(instr_ROR),    addr(addr_mode_ZPX),     2,    6);
    set_opcode(0X78,     "SEI",     instr(instr_SEI),    addr(addr_mode_IMP),     1,    2);
    set_opcode(0X79,     "ADC",     instr(instr_ADC),    addr(addr_mode_ABSY),     3,    4);
    set_opcode(0X7D,     "ADC",     instr(instr_ADC),    addr(addr_mode_ABSX),     3,    4);
    set_opcode(0X7E,     "ROR",     instr(instr_ROR),    addr(addr_mode_ABS),     3,    6);
        
    // Row 8    
    set_opcode(0X81,     "STA",     instr(instr_STA),    addr(addr_mode_INDX),     2,    6);
    set_opcode(0X84,     "STY",     instr(instr_STY),    addr(addr_mode_ZP),     2,    3);
    set_opcode(0X85,     "STA",     instr(instr_STA),    addr(addr_mode_ZP),     2,    3);
    set_opcode(0X86,     "STX",     instr(instr_STX),    addr(addr_mode_ZP),     2,    3);
    set_opcode(0X88,     "DEY",     instr(instr_DEY),    addr(addr_mode_IMP),     1,    2);
    set_opcode(0X8A,     "TXA",     instr(instr_TXA),    addr(addr_mode_IMP),     1,    2);
    set_opcode(0X8C,     "STY",     instr(instr_STY),    addr(addr_mode_ABS),     3,    4);
    set_opcode(0X8D,     "STA",     instr(instr_STA),    addr(addr_mode_ABS),     3,    4);
    set_opcode(0X8E,     "STX",     instr(instr_STX),    addr(addr_mode_ABS),     3,    4);
    
    // Row 9    
    set_opcode(0X90,     "BCC",     instr(instr_BCC),    addr(addr_mode_REL),     2,    2);
    set_opcode(0X91,     "STA",     instr(instr_STA),    addr(addr_mode_INDY),     2,    6);
    set_opcode(0X94,     "STY",     instr(instr_STY),    addr(addr_mode_ZPX),     2,    4);
    set_opcode(0X95,     "STA",     instr(instr_STA),    addr(addr_mode_ZPX),     2,    4);
    set_opcode(0X96,     "STX",     instr(instr_STX),    addr(addr_mode_ZPY),     2,    4);
    set_opcode(0X98,     "TYA",     instr(instr_TYA),    addr(addr_mode_IMP),     1,    2);
    set_opcode(0X99,     "STA",     instr(instr_STA),    addr(addr_mode_ABSY),     3,    5);
    set_opcode(0X9A,     "TXS",     instr(instr_TXS),    addr(addr_mode_IMP),     1,    2);
    set_opcode(0X9D,     "STA",     instr(instr_STA),    addr(addr_mode_ABSX),     3,    5);
    
    // Row A    
    set_opcode(0XA0,     "LDY",     instr(instr_LDY),    addr(addr_mode_IMM),     2,    2);
    set_opcode(0XA1,     "LDA",     instr(instr_LDA),    addr(addr_mode_INDX),     2,    6);
    set_opcode(0XA2,     "LDX",     instr(instr_LDX),    addr(addr_mode_IMM),     2,    2);
    set_opcode(0XA4,     "LDY",     instr(instr_LDY),    addr(addr_mode_ZP),     2,    3);
    set_opcode(0XA5,     "LDA",     instr(instr_LDA),    addr(addr_mode_ZP),     2,    3);
    set_opcode(0XA6,     "LDX",     instr(instr_LDX),    addr(addr_mode_ZP),     2,    3);
    set_opcode(0XA8,     "TAY",     instr(instr_TAY),    addr(addr_mode_IMP),     1,    2);
    set_opcode(0XA9,     "LDA",     instr(instr_LDA),    addr(addr_mode_IMM),     2,    2);
    set_opcode(0XAA,     "TAX",     instr(instr_TAX),    addr(addr_mode_IMP),     1,    2);
    set_opcode(0XAC,     "LDY",     instr(instr_LDY),    addr(addr_mode_ABS),     3,    4);
    set_opcode(0XAD,     "LDA",     instr(instr_LDA),    addr(addr_mode_ABS),     3,    4);
    set_opcode(0XAE,     "LDX",     instr(instr_LDX),    addr(addr_mode_ABS),     3,    4);
    
    // Row B    
    set_opcode(0XB0,     "BCS",     instr(instr_BCS),    addr(addr_mode_REL),     2,    2);
    set_opcode(0XB1,     "LDA",     instr(instr_LDA),    addr(addr_mode_INDY),     2,    5);
    set_opcode(0XB4,     "LDY",     instr(instr_LDY),    addr(addr_mode_ZPX),     2,    4);
    set_opcode(0XB5,     "LDA",     instr(instr_LDA),    addr(addr_mode_ZPX),     2,    4);
    set_opcode(0XB6,     "LDX",     instr(instr_LDX),    addr(addr_mode_ZPY),     2,    4);
    set_opcode(0XB8,     "CLV",     instr(instr_CLV),    addr(addr_mode_IMP),     1,    2);
    set_opcode(0XB9,     "LDA",     instr(instr_LDA),    addr(addr_mode_ABSY),     3,    4);
    set_opcode(0XBA,     "TSX",     instr(instr_TSX),    addr(addr_mode_IMP),     1,    2);
    set_opcode(0XBC,     "LDY",     instr(instr_LDY),    addr(addr_mode_ABSX),     3,    4);
    set_opcode(0XBD,     "LDA",     instr(instr_LDA),    addr(addr_mode_ABSX),     3,    4);
    set_opcode(0XBE,     "LDX",     instr(instr_LDX),    addr(addr_mode_ABSY),     3,    4);
    
    // Row C    
    set_opcode(0XC0,     "CPY",     instr(instr_CPY),    addr(addr_mode_IMM),     2,    2);
    set_opcode(0XC1,     "CMP",     instr(instr_CMP),    addr(addr_mode_INDX),     2,    6);
    set_opcode(0XC4,     "CPY",     instr(instr_CPY),    addr(addr_mode_ZP),     2,    3);
    set_opcode(0XC5,     "CMP",     instr(instr_CMP),    addr(addr_mode_ZP),     2,    3);
    set_opcode(0XC6,     "DEC",     instr(instr_DEC),    addr(addr_mode_ZP),     2,    5);
    set_opcode(0XC8,     "INY",     instr(instr_INY),    addr(addr_mode_IMP),     1,    2);
    set_opcode(0XC9,     "CMP",     instr(instr_CMP),    addr(addr_mode_IMM),     2,    2);
    set_opcode(0XCA,     "DEX",     instr(instr_DEX),    addr(addr_mode_IMP),     1,    2);
    set_opcode(0XCC,     "CPY",     instr(instr_CPY),    addr(addr_mode_ABS),     3,    4);
    set_opcode(0XCD,     "CMP",     instr(instr_CMP),    addr(addr_mode_ABS),     3,    4);
    set_opcode(0XCE,     "DEC",     instr(instr_DEC),    addr(addr_mode_ABS),     3,    6);
    
    // Row D    
    set_opcode(0XD0,     "BNE",     instr(instr_BNE),    addr(addr_mode_REL),     2,    2);
    set_opcode(0XD1,     "CMP",     instr(instr_CMP),    addr(addr_mode_INDY),     2,    5);
    set_opcode(0XD5,     "CMP",     instr(instr_CMP),    addr(addr_mode_ZPX),     2,    4);
    set_opcode(0XD6,     "DEC",     instr(instr_DEC),    addr(addr_mode_ZPX),     2,    6);
    set_opcode(0XD8,     "CLD",     instr(instr_CLD),    addr(addr_mode_IMP),     1,    2);
    set_opcode(0XD9,     "CMP",     instr(instr_CMP),    addr(addr_mode_ABSY),     3,    4);
    set_opcode(0XDD,     "CMP",     instr(instr_CMP),    addr(addr_mode_ABSX),     3,    4);
    set_opcode(0XDE,     "DEC",     instr(instr_DEC),    addr(addr_mode_ABSX),     3,    7);
    
    // Row E    
    set_opcode(0XE0,     "CPX",     instr(instr_CPX),    addr(addr_mode_IMM),     2,    2);
    set_opcode(0XE1,     "SBC",     instr(instr_SBC),    addr(addr_mode_INDX),     2,    6);
    set_opcode(0XE4,     "CPX",     instr(instr_CPX),    addr(addr_mode_ZP),     2,    3);
    set_opcode(0XE5,     "SBC",     instr(instr_SBC),    addr(addr_mode_ZP),     2,    3);
    set_opcode(0XE6,     "INC",     instr(instr_INC),    addr(addr_mode_ZP),     2,    5);
    set_opcode(0XE8,     "INX",     instr(instr_INX),    addr(addr_mode_IMP),     1,    2);
    set_opcode(0XE9,     "SBC",     instr(instr_SBC),    addr(addr_mode_IMM),     2,    2);
    set_opcode(0XEA,     "NOP",     instr(instr_NOP),    addr(addr_mode_IMP),     1,    2);
    set_opcode(0XEC,     "CPX",     instr(instr_CPX),    addr(addr_mode_ABS),     3,    4);
    set_opcode(0XED,     "SBC",     instr(instr_SBC),    addr(addr_mode_ABS),     3,    4);
    set_opcode(0XEE,     "INC",     instr(instr_INC),    addr(addr_mode_ABS),     3,    6);
    set_opcode(0XF0,     "BEQ",     instr(instr_BEQ),    addr(addr_mode_REL),     2,    2);
    
    // Row F    
    set_opcode(0XF1,     "SBC",     instr(instr_SBC),    addr(addr_mode_INDY),     2,    5);
    set_opcode(0XF5,     "SBC",     instr(instr_SBC),    addr(addr_mode_ZPX),     2,    4);
    set_opcode(0XF6,     "INC",     instr(instr_INC),    addr(addr_mode_ZPX),     2,    6);
    set_opcode(0XF8,     "SED",     instr(instr_SED),    addr(addr_mode_IMP),     1,    2);
    set_opcode(0XF9,     "SBC",     instr(instr_SBC),    addr(addr_mode_ABSY),     3,    4);
    set_opcode(0XFD,     "SBC",     instr(instr_SBC),    addr(addr_mode_ABSX),     3,    4);
    set_opcode(0XFE,     "INC",     instr(instr_INC),    addr(addr_mode_ABSX),     3,    7);        
}   

void cpu::set_opcode(uint8_t index, std::string name, std::function<uint8_t(void)> instruction, std::function<uint8_t(void)> address_mode, uint8_t instruction_bytes, uint8_t cycles_needed) {
    _opcode_decoder_lookup[index].name = name;
    _opcode_decoder_lookup[index].instruction = instruction;
    _opcode_decoder_lookup[index].address_mode = address_mode;
    _opcode_decoder_lookup[index].instruction_bytes = instruction_bytes;
    _opcode_decoder_lookup[index].cycles_needed = cycles_needed;
}
