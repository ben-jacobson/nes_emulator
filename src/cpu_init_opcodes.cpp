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
    set_opcode(0x00,     "BRK",     instr(instr_BRK),    addr(addr_mode_IMP),     1,    7);
    set_opcode(0x01,     "ORA",     instr(instr_ORA),    addr(addr_mode_INDX),     2,    6);
    set_opcode(0x05,     "ORA",     instr(instr_ORA),    addr(addr_mode_ZP),     2,    3);
    set_opcode(0x06,     "ASL",     instr(instr_ASL),    addr(addr_mode_ZP),     2,    5);
    set_opcode(0x08,     "PHP",     instr(instr_PHP),    addr(addr_mode_IMP),     1,    3);
    set_opcode(0x09,     "ORA",     instr(instr_ORA),    addr(addr_mode_IMM),     2,    2);
    set_opcode(0x0A,     "ASL",     instr(instr_ASL),    addr(addr_mode_ACC),     1,    2);
    set_opcode(0x0D,     "ORA",     instr(instr_ORA),    addr(addr_mode_ABS),     3,    4);
    set_opcode(0x0E,     "ASL",     instr(instr_ASL),    addr(addr_mode_ABS),     3,    6);    
    
    // Row 1    
    set_opcode(0x10,     "BPL",     instr(instr_BPL),    addr(addr_mode_REL),     2,    2);
    set_opcode(0x11,     "ORA",     instr(instr_ORA),    addr(addr_mode_INDY),     2,    5);
    set_opcode(0x15,     "ORA",     instr(instr_ORA),    addr(addr_mode_ZPX),     2,    4);
    set_opcode(0x16,     "ASL",     instr(instr_ASL),    addr(addr_mode_ZPX),     2,    6);
    set_opcode(0x18,     "CLC",     instr(instr_CLC),    addr(addr_mode_IMP),     1,    2);
    set_opcode(0x19,     "ORA",     instr(instr_ORA),    addr(addr_mode_ABSY),     3,    4);
    set_opcode(0x1D,     "ORA",     instr(instr_ORA),    addr(addr_mode_ABSX),     3,    4);
    set_opcode(0x1E,     "ASL",     instr(instr_ASL),    addr(addr_mode_ABSX),     3,    7);
    
    // Row 2    
    set_opcode(0x20,     "JSR",     instr(instr_JSR),    addr(addr_mode_ABS),     3,    6);
    set_opcode(0x21,     "AND",     instr(instr_AND),    addr(addr_mode_INDX),     2,    6);
    set_opcode(0x24,     "BIT",     instr(instr_BIT),    addr(addr_mode_ZP),     2,    3);
    set_opcode(0x25,     "AND",     instr(instr_AND),    addr(addr_mode_ZP),     2,    3);
    set_opcode(0x26,     "ROL",     instr(instr_ROL),    addr(addr_mode_ZP),     2,    5);
    set_opcode(0x28,     "PLP",     instr(instr_PLP),    addr(addr_mode_IMP),     1,    4);
    set_opcode(0x29,     "AND",     instr(instr_AND),    addr(addr_mode_IMM),     2,    2);
    set_opcode(0x2A,     "ROL",     instr(instr_ROL),    addr(addr_mode_ACC),     1,    2);
    set_opcode(0x2C,     "BIT",     instr(instr_BIT),    addr(addr_mode_ABS),     3,    4);
    set_opcode(0x2D,     "AND",     instr(instr_AND),    addr(addr_mode_ABS),     3,    4);
    set_opcode(0x2E,     "ROL",     instr(instr_ROL),    addr(addr_mode_ABS),     3,    6);
    
    // Row 3    
    set_opcode(0x30,     "BMI",     instr(instr_BMI),    addr(addr_mode_REL),     2,    2);
    set_opcode(0x31,     "AND",     instr(instr_AND),    addr(addr_mode_INDY),     2,    5);
    set_opcode(0x35,     "AND",     instr(instr_AND),    addr(addr_mode_ZPX),     2,    4);
    set_opcode(0x36,     "ROL",     instr(instr_ROL),    addr(addr_mode_ZPX),     2,    6);
    set_opcode(0x38,     "SEC",     instr(instr_SEC),    addr(addr_mode_IMP),     1,    2);
    set_opcode(0x39,     "AND",     instr(instr_AND),    addr(addr_mode_ABSY),     3,    4);
    set_opcode(0x3D,     "AND",     instr(instr_AND),    addr(addr_mode_ABSX),     3,    4);
    set_opcode(0x3E,     "ROL",     instr(instr_ROL),    addr(addr_mode_ABSX),     3,    7);
    
    // Row 4    
    set_opcode(0x40,     "RTI",     instr(instr_RTI),    addr(addr_mode_IMP),     1,    6);
    set_opcode(0x41,     "EOR",     instr(instr_EOR),    addr(addr_mode_INDX),     2,    6);
    set_opcode(0x45,     "EOR",     instr(instr_EOR),    addr(addr_mode_ZP),     2,    3);
    set_opcode(0x46,     "LSR",     instr(instr_LSR),    addr(addr_mode_ZP),     2,    5);
    set_opcode(0x48,     "PHA",     instr(instr_PHA),    addr(addr_mode_IMP),     1,    3);
    set_opcode(0x49,     "EOR",     instr(instr_EOR),    addr(addr_mode_IMM),     2,    2);
    set_opcode(0x4A,     "LSR",     instr(instr_LSR),    addr(addr_mode_ACC),     1,    2);
    set_opcode(0x4C,     "JMP",     instr(instr_JMP),    addr(addr_mode_ABS),     3,    3);
    set_opcode(0x4D,     "EOR",     instr(instr_EOR),    addr(addr_mode_ABS),     3,    4);
    set_opcode(0x4E,     "LSR",     instr(instr_LSR),    addr(addr_mode_ABS),     3,    6);
    
    // Row 5    
    set_opcode(0x50,     "BVC",     instr(instr_BVC),    addr(addr_mode_REL),     2,    2);
    set_opcode(0x51,     "EOR",     instr(instr_EOR),    addr(addr_mode_INDY),     2,    5);
    set_opcode(0x55,     "EOR",     instr(instr_EOR),    addr(addr_mode_ZPX),     2,    4);
    set_opcode(0x56,     "LSR",     instr(instr_LSR),    addr(addr_mode_ZPX),     2,    6);
    set_opcode(0x58,     "CLI",     instr(instr_CLI),    addr(addr_mode_IMP),     1,    2);
    set_opcode(0x59,     "EOR",     instr(instr_EOR),    addr(addr_mode_ABSY),     3,    4);
    set_opcode(0x5D,     "EOR",     instr(instr_EOR),    addr(addr_mode_ABSX),     3,    4);
    set_opcode(0x5E,     "LSR",     instr(instr_LSR),    addr(addr_mode_ABSX),     3,    7);
    
    // Row 6    
    set_opcode(0x60,     "RTS",     instr(instr_RTS),    addr(addr_mode_IMP),     1,    6);
    set_opcode(0x61,     "ADC",     instr(instr_ADC),    addr(addr_mode_INDX),     2,    6);
    set_opcode(0x65,     "ADC",     instr(instr_ADC),    addr(addr_mode_ZP),     2,    3);
    set_opcode(0x66,     "ROR",     instr(instr_ROR),    addr(addr_mode_ZP),     2,    5);
    set_opcode(0x68,     "PLA",     instr(instr_PLA),    addr(addr_mode_IMP),     1,    4);
    set_opcode(0x69,     "ADC",     instr(instr_ADC),    addr(addr_mode_IMM),     2,    2);
    set_opcode(0x6A,     "ROR",     instr(instr_ROR),    addr(addr_mode_ACC),     1,    2);
    set_opcode(0x6C,     "JMP",     instr(instr_JMP),    addr(addr_mode_INDI),     3,    5);
    set_opcode(0x6D,     "ADC",     instr(instr_ADC),    addr(addr_mode_ABS),     3,    4);
    set_opcode(0x6E,     "ROR",     instr(instr_ROR),    addr(addr_mode_ABSX),     3,    7);
    
    // Row 7    
    set_opcode(0x70,     "BVS",     instr(instr_BVS),    addr(addr_mode_REL),     2,    2);
    set_opcode(0x71,     "ADC",     instr(instr_ADC),    addr(addr_mode_INDY),     2,    5);
    set_opcode(0x75,     "ADC",     instr(instr_ADC),    addr(addr_mode_ZPX),     2,    4);
    set_opcode(0x76,     "ROR",     instr(instr_ROR),    addr(addr_mode_ZPX),     2,    6);
    set_opcode(0x78,     "SEI",     instr(instr_SEI),    addr(addr_mode_IMP),     1,    2);
    set_opcode(0x79,     "ADC",     instr(instr_ADC),    addr(addr_mode_ABSY),     3,    4);
    set_opcode(0x7D,     "ADC",     instr(instr_ADC),    addr(addr_mode_ABSX),     3,    4);
    set_opcode(0x7E,     "ROR",     instr(instr_ROR),    addr(addr_mode_ABS),     3,    6);
        
    // Row 8    
    set_opcode(0x81,     "STA",     instr(instr_STA),    addr(addr_mode_INDX),     2,    6);
    set_opcode(0x84,     "STY",     instr(instr_STY),    addr(addr_mode_ZP),     2,    3);
    set_opcode(0x85,     "STA",     instr(instr_STA),    addr(addr_mode_ZP),     2,    3);
    set_opcode(0x86,     "STX",     instr(instr_STX),    addr(addr_mode_ZP),     2,    3);
    set_opcode(0x88,     "DEY",     instr(instr_DEY),    addr(addr_mode_IMP),     1,    2);
    set_opcode(0x8A,     "TXA",     instr(instr_TXA),    addr(addr_mode_IMP),     1,    2);
    set_opcode(0x8C,     "STY",     instr(instr_STY),    addr(addr_mode_ABS),     3,    4);
    set_opcode(0x8D,     "STA",     instr(instr_STA),    addr(addr_mode_ABS),     3,    4);
    set_opcode(0x8E,     "STX",     instr(instr_STX),    addr(addr_mode_ABS),     3,    4);
    
    // Row 9    
    set_opcode(0x90,     "BCC",     instr(instr_BCC),    addr(addr_mode_REL),     2,    2);
    set_opcode(0x91,     "STA",     instr(instr_STA),    addr(addr_mode_INDY),     2,    6);
    set_opcode(0x94,     "STY",     instr(instr_STY),    addr(addr_mode_ZPX),     2,    4);
    set_opcode(0x95,     "STA",     instr(instr_STA),    addr(addr_mode_ZPX),     2,    4);
    set_opcode(0x96,     "STX",     instr(instr_STX),    addr(addr_mode_ZPY),     2,    4);
    set_opcode(0x98,     "TYA",     instr(instr_TYA),    addr(addr_mode_IMP),     1,    2);
    set_opcode(0x99,     "STA",     instr(instr_STA),    addr(addr_mode_ABSY),     3,    5);
    set_opcode(0x9A,     "TXS",     instr(instr_TXS),    addr(addr_mode_IMP),     1,    2);
    set_opcode(0x9D,     "STA",     instr(instr_STA),    addr(addr_mode_ABSX),     3,    5);
    
    // Row A    
    set_opcode(0xA0,     "LDY",     instr(instr_LDY),    addr(addr_mode_IMM),     2,    2);
    set_opcode(0xA1,     "LDA",     instr(instr_LDA),    addr(addr_mode_INDX),     2,    6);
    set_opcode(0xA2,     "LDX",     instr(instr_LDX),    addr(addr_mode_IMM),     2,    2);
    set_opcode(0xA4,     "LDY",     instr(instr_LDY),    addr(addr_mode_ZP),     2,    3);
    set_opcode(0xA5,     "LDA",     instr(instr_LDA),    addr(addr_mode_ZP),     2,    3);
    set_opcode(0xA6,     "LDX",     instr(instr_LDX),    addr(addr_mode_ZP),     2,    3);
    set_opcode(0xA8,     "TAY",     instr(instr_TAY),    addr(addr_mode_IMP),     1,    2);
    set_opcode(0xA9,     "LDA",     instr(instr_LDA),    addr(addr_mode_IMM),     2,    2);
    set_opcode(0xAA,     "TAX",     instr(instr_TAX),    addr(addr_mode_IMP),     1,    2);
    set_opcode(0xAC,     "LDY",     instr(instr_LDY),    addr(addr_mode_ABS),     3,    4);
    set_opcode(0xAD,     "LDA",     instr(instr_LDA),    addr(addr_mode_ABS),     3,    4);
    set_opcode(0xAE,     "LDX",     instr(instr_LDX),    addr(addr_mode_ABS),     3,    4);
    
    // Row B    
    set_opcode(0xB0,     "BCS",     instr(instr_BCS),    addr(addr_mode_REL),     2,    2);
    set_opcode(0xB1,     "LDA",     instr(instr_LDA),    addr(addr_mode_INDY),     2,    5);
    set_opcode(0xB4,     "LDY",     instr(instr_LDY),    addr(addr_mode_ZPX),     2,    4);
    set_opcode(0xB5,     "LDA",     instr(instr_LDA),    addr(addr_mode_ZPX),     2,    4);
    set_opcode(0xB6,     "LDX",     instr(instr_LDX),    addr(addr_mode_ZPY),     2,    4);
    set_opcode(0xB8,     "CLV",     instr(instr_CLV),    addr(addr_mode_IMP),     1,    2);
    set_opcode(0xB9,     "LDA",     instr(instr_LDA),    addr(addr_mode_ABSY),     3,    4);
    set_opcode(0xBA,     "TSX",     instr(instr_TSX),    addr(addr_mode_IMP),     1,    2);
    set_opcode(0xBC,     "LDY",     instr(instr_LDY),    addr(addr_mode_ABSX),     3,    4);
    set_opcode(0xBD,     "LDA",     instr(instr_LDA),    addr(addr_mode_ABSX),     3,    4);
    set_opcode(0xBE,     "LDX",     instr(instr_LDX),    addr(addr_mode_ABSY),     3,    4);
    
    // Row C    
    set_opcode(0xC0,     "CPY",     instr(instr_CPY),    addr(addr_mode_IMM),     2,    2);
    set_opcode(0xC1,     "CMP",     instr(instr_CMP),    addr(addr_mode_INDX),     2,    6);
    set_opcode(0xC4,     "CPY",     instr(instr_CPY),    addr(addr_mode_ZP),     2,    3);
    set_opcode(0xC5,     "CMP",     instr(instr_CMP),    addr(addr_mode_ZP),     2,    3);
    set_opcode(0xC6,     "DEC",     instr(instr_DEC),    addr(addr_mode_ZP),     2,    5);
    set_opcode(0xC8,     "INY",     instr(instr_INY),    addr(addr_mode_IMP),     1,    2);
    set_opcode(0xC9,     "CMP",     instr(instr_CMP),    addr(addr_mode_IMM),     2,    2);
    set_opcode(0xCA,     "DEX",     instr(instr_DEX),    addr(addr_mode_IMP),     1,    2);
    set_opcode(0xCC,     "CPY",     instr(instr_CPY),    addr(addr_mode_ABS),     3,    4);
    set_opcode(0xCD,     "CMP",     instr(instr_CMP),    addr(addr_mode_ABS),     3,    4);
    set_opcode(0xCE,     "DEC",     instr(instr_DEC),    addr(addr_mode_ABS),     3,    6);
    
    // Row D    
    set_opcode(0xD0,     "BNE",     instr(instr_BNE),    addr(addr_mode_REL),     2,    2);
    set_opcode(0xD1,     "CMP",     instr(instr_CMP),    addr(addr_mode_INDY),     2,    5);
    set_opcode(0xD5,     "CMP",     instr(instr_CMP),    addr(addr_mode_ZPX),     2,    4);
    set_opcode(0xD6,     "DEC",     instr(instr_DEC),    addr(addr_mode_ZPX),     2,    6);
    set_opcode(0xD8,     "CLD",     instr(instr_CLD),    addr(addr_mode_IMP),     1,    2);
    set_opcode(0xD9,     "CMP",     instr(instr_CMP),    addr(addr_mode_ABSY),     3,    4);
    set_opcode(0xDD,     "CMP",     instr(instr_CMP),    addr(addr_mode_ABSX),     3,    4);
    set_opcode(0xDE,     "DEC",     instr(instr_DEC),    addr(addr_mode_ABSX),     3,    7);
    
    // Row E    
    set_opcode(0xE0,     "CPX",     instr(instr_CPX),    addr(addr_mode_IMM),     2,    2);
    set_opcode(0xE1,     "SBC",     instr(instr_SBC),    addr(addr_mode_INDX),     2,    6);
    set_opcode(0xE4,     "CPX",     instr(instr_CPX),    addr(addr_mode_ZP),     2,    3);
    set_opcode(0xE5,     "SBC",     instr(instr_SBC),    addr(addr_mode_ZP),     2,    3);
    set_opcode(0xE6,     "INC",     instr(instr_INC),    addr(addr_mode_ZP),     2,    5);
    set_opcode(0xE8,     "INX",     instr(instr_INX),    addr(addr_mode_IMP),     1,    2);
    set_opcode(0xE9,     "SBC",     instr(instr_SBC),    addr(addr_mode_IMM),     2,    2);
    set_opcode(0xEA,     "NOP",     instr(instr_NOP),    addr(addr_mode_IMP),     1,    2);
    set_opcode(0xEC,     "CPX",     instr(instr_CPX),    addr(addr_mode_ABS),     3,    4);
    set_opcode(0xED,     "SBC",     instr(instr_SBC),    addr(addr_mode_ABS),     3,    4);
    set_opcode(0xEE,     "INC",     instr(instr_INC),    addr(addr_mode_ABS),     3,    6);
    set_opcode(0xF0,     "BEQ",     instr(instr_BEQ),    addr(addr_mode_REL),     2,    2);
    
    // Row F    
    set_opcode(0xF1,     "SBC",     instr(instr_SBC),    addr(addr_mode_INDY),     2,    5);
    set_opcode(0xF5,     "SBC",     instr(instr_SBC),    addr(addr_mode_ZPX),     2,    4);
    set_opcode(0xF6,     "INC",     instr(instr_INC),    addr(addr_mode_ZPX),     2,    6);
    set_opcode(0xF8,     "SED",     instr(instr_SED),    addr(addr_mode_IMP),     1,    2);
    set_opcode(0xF9,     "SBC",     instr(instr_SBC),    addr(addr_mode_ABSY),     3,    4);
    set_opcode(0xFD,     "SBC",     instr(instr_SBC),    addr(addr_mode_ABSX),     3,    4);
    set_opcode(0xFE,     "INC",     instr(instr_INC),    addr(addr_mode_ABSX),     3,    7);        
}   

void cpu::set_opcode(uint8_t index, std::string name, std::function<uint8_t(void)> instruction, std::function<uint8_t(void)> address_mode, uint8_t instruction_bytes, uint8_t cycles_needed) {
    _opcode_decoder_lookup[index].name = name;
    _opcode_decoder_lookup[index].instruction = instruction;
    _opcode_decoder_lookup[index].address_mode = address_mode;
    _opcode_decoder_lookup[index].instruction_bytes = instruction_bytes;
    _opcode_decoder_lookup[index].cycles_needed = cycles_needed;
}
