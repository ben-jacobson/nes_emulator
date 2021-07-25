#include "cpu.h"

// a huge thank you to https://github.com/kpmiller for his emulator101 repo, containing an easy to copy matrix of opcodes

#define instr(fn)   std::bind(&cpu::fn, this)
#define addr(fn)    std::bind(&cpu::fn, this)

void cpu::init_opcode_decoder_lookup(void) {
    // start by filling out the illegal opcodes
    for (uint16_t i = 0; i < OPCODE_COUNT; i++) {
        set_opcode(i, instr(instr_ZZZ), "ZZZ", addr(addr_mode_IMP), "ZZZ", 1, 2); 
    }
    

    // then we'll build out the ones we want
    // I built this code in Excel, my second favourite IDE :)

    // Row 0    
    set_opcode(0x00, instr(instr_BRK), "BRK", addr(addr_mode_IMP), "IMP", 1, 7);
    set_opcode(0x01, instr(instr_ORA), "ORA", addr(addr_mode_INDX), "INDX", 2, 6);
    set_opcode(0x05, instr(instr_ORA), "ORA", addr(addr_mode_ZP), "ZP", 2, 3);
    set_opcode(0x06, instr(instr_ASL), "ASL", addr(addr_mode_ZP), "ZP", 2, 5);
    set_opcode(0x08, instr(instr_PHP), "PHP", addr(addr_mode_IMP), "IMP", 1, 3);
    set_opcode(0x09, instr(instr_ORA), "ORA", addr(addr_mode_IMM), "IMM", 2, 2);
    set_opcode(0x0A, instr(instr_ASL), "ASL", addr(addr_mode_ACC), "ACC", 1, 2);
    set_opcode(0x0D, instr(instr_ORA), "ORA", addr(addr_mode_ABS), "ABS", 3, 4);
    set_opcode(0x0E, instr(instr_ASL), "ASL", addr(addr_mode_ABS), "ABS", 3, 6);
        
    // 1 Row    
    set_opcode(0x10, instr(instr_BPL), "BPL", addr(addr_mode_REL), "REL", 2, 2);
    set_opcode(0x11, instr(instr_ORA), "ORA", addr(addr_mode_INDY), "INDY", 2, 5);
    set_opcode(0x15, instr(instr_ORA), "ORA", addr(addr_mode_ZPX), "ZPX", 2, 4);
    set_opcode(0x16, instr(instr_ASL), "ASL", addr(addr_mode_ZPX), "ZPX", 2, 6);
    set_opcode(0x18, instr(instr_CLC), "CLC", addr(addr_mode_IMP), "IMP", 1, 2);
    set_opcode(0x19, instr(instr_ORA), "ORA", addr(addr_mode_ABSY), "ABSY", 3, 4);
    set_opcode(0x1D, instr(instr_ORA), "ORA", addr(addr_mode_ABSX), "ABSX", 3, 4);
    set_opcode(0x1E, instr(instr_ASL), "ASL", addr(addr_mode_ABSX), "ABSX", 3, 7);
        
    // 2 Row    
    set_opcode(0x20, instr(instr_JSR), "JSR", addr(addr_mode_ABS), "ABS", 3, 6);
    set_opcode(0x21, instr(instr_AND), "AND", addr(addr_mode_INDX), "INDX", 2, 6);
    set_opcode(0x24, instr(instr_BIT), "BIT", addr(addr_mode_ZP), "ZP", 2, 3);
    set_opcode(0x25, instr(instr_AND), "AND", addr(addr_mode_ZP), "ZP", 2, 3);
    set_opcode(0x26, instr(instr_ROL), "ROL", addr(addr_mode_ZP), "ZP", 2, 5);
    set_opcode(0x28, instr(instr_PLP), "PLP", addr(addr_mode_IMP), "IMP", 1, 4);
    set_opcode(0x29, instr(instr_AND), "AND", addr(addr_mode_IMM), "IMM", 2, 2);
    set_opcode(0x2A, instr(instr_ROL), "ROL", addr(addr_mode_ACC), "ACC", 1, 2);
    set_opcode(0x2C, instr(instr_BIT), "BIT", addr(addr_mode_ABS), "ABS", 3, 4);
    set_opcode(0x2D, instr(instr_AND), "AND", addr(addr_mode_ABS), "ABS", 3, 4);
    set_opcode(0x2E, instr(instr_ROL), "ROL", addr(addr_mode_ABS), "ABS", 3, 6);
        
    // 3 Row    
    set_opcode(0x30, instr(instr_BMI), "BMI", addr(addr_mode_REL), "REL", 2, 2);
    set_opcode(0x31, instr(instr_AND), "AND", addr(addr_mode_INDY), "INDY", 2, 5);
    set_opcode(0x35, instr(instr_AND), "AND", addr(addr_mode_ZPX), "ZPX", 2, 4);
    set_opcode(0x36, instr(instr_ROL), "ROL", addr(addr_mode_ZPX), "ZPX", 2, 6);
    set_opcode(0x38, instr(instr_SEC), "SEC", addr(addr_mode_IMP), "IMP", 1, 2);
    set_opcode(0x39, instr(instr_AND), "AND", addr(addr_mode_ABSY), "ABSY", 3, 4);
    set_opcode(0x3D, instr(instr_AND), "AND", addr(addr_mode_ABSX), "ABSX", 3, 4);
    set_opcode(0x3E, instr(instr_ROL), "ROL", addr(addr_mode_ABSX), "ABSX", 3, 7);
        
    // 4 Row    
    set_opcode(0x40, instr(instr_RTI), "RTI", addr(addr_mode_IMP), "IMP", 1, 6);
    set_opcode(0x41, instr(instr_EOR), "EOR", addr(addr_mode_INDX), "INDX", 2, 6);
    set_opcode(0x45, instr(instr_EOR), "EOR", addr(addr_mode_ZP), "ZP", 2, 3);
    set_opcode(0x46, instr(instr_LSR), "LSR", addr(addr_mode_ZP), "ZP", 2, 5);
    set_opcode(0x48, instr(instr_PHA), "PHA", addr(addr_mode_IMP), "IMP", 1, 3);
    set_opcode(0x49, instr(instr_EOR), "EOR", addr(addr_mode_IMM), "IMM", 2, 2);
    set_opcode(0x4A, instr(instr_LSR), "LSR", addr(addr_mode_ACC), "ACC", 1, 2);
    set_opcode(0x4C, instr(instr_JMP), "JMP", addr(addr_mode_ABS), "ABS", 3, 3);
    set_opcode(0x4D, instr(instr_EOR), "EOR", addr(addr_mode_ABS), "ABS", 3, 4);
    set_opcode(0x4E, instr(instr_LSR), "LSR", addr(addr_mode_ABS), "ABS", 3, 6);
        
    // 5 Row    
    set_opcode(0x50, instr(instr_BVC), "BVC", addr(addr_mode_REL), "REL", 2, 2);
    set_opcode(0x51, instr(instr_EOR), "EOR", addr(addr_mode_INDY), "INDY", 2, 5);
    set_opcode(0x55, instr(instr_EOR), "EOR", addr(addr_mode_ZPX), "ZPX", 2, 4);
    set_opcode(0x56, instr(instr_LSR), "LSR", addr(addr_mode_ZPX), "ZPX", 2, 6);
    set_opcode(0x58, instr(instr_CLI), "CLI", addr(addr_mode_IMP), "IMP", 1, 2);
    set_opcode(0x59, instr(instr_EOR), "EOR", addr(addr_mode_ABSY), "ABSY", 3, 4);
    set_opcode(0x5D, instr(instr_EOR), "EOR", addr(addr_mode_ABSX), "ABSX", 3, 4);
    set_opcode(0x5E, instr(instr_LSR), "LSR", addr(addr_mode_ABSX), "ABSX", 3, 7);
        
    // 6 Row    
    set_opcode(0x60, instr(instr_RTS), "RTS", addr(addr_mode_IMP), "IMP", 1, 6);
    set_opcode(0x61, instr(instr_ADC), "ADC", addr(addr_mode_INDX), "INDX", 2, 6);
    set_opcode(0x65, instr(instr_ADC), "ADC", addr(addr_mode_ZP), "ZP", 2, 3);
    set_opcode(0x66, instr(instr_ROR), "ROR", addr(addr_mode_ZP), "ZP", 2, 5);
    set_opcode(0x68, instr(instr_PLA), "PLA", addr(addr_mode_IMP), "IMP", 1, 4);
    set_opcode(0x69, instr(instr_ADC), "ADC", addr(addr_mode_IMM), "IMM", 2, 2);
    set_opcode(0x6A, instr(instr_ROR), "ROR", addr(addr_mode_ACC), "ACC", 1, 2);
    set_opcode(0x6C, instr(instr_JMP), "JMP", addr(addr_mode_INDI), "INDI", 3, 5);
    set_opcode(0x6D, instr(instr_ADC), "ADC", addr(addr_mode_ABS), "ABS", 3, 4);
    set_opcode(0x6E, instr(instr_ROR), "ROR", addr(addr_mode_ABSX), "ABSX", 3, 7);
        
    // 7 Row    
    set_opcode(0x70, instr(instr_BVS), "BVS", addr(addr_mode_REL), "REL", 2, 2);
    set_opcode(0x71, instr(instr_ADC), "ADC", addr(addr_mode_INDY), "INDY", 2, 5);
    set_opcode(0x75, instr(instr_ADC), "ADC", addr(addr_mode_ZPX), "ZPX", 2, 4);
    set_opcode(0x76, instr(instr_ROR), "ROR", addr(addr_mode_ZPX), "ZPX", 2, 6);
    set_opcode(0x78, instr(instr_SEI), "SEI", addr(addr_mode_IMP), "IMP", 1, 2);
    set_opcode(0x79, instr(instr_ADC), "ADC", addr(addr_mode_ABSY), "ABSY", 3, 4);
    set_opcode(0x7D, instr(instr_ADC), "ADC", addr(addr_mode_ABSX), "ABSX", 3, 4);
    set_opcode(0x7E, instr(instr_ROR), "ROR", addr(addr_mode_ABS), "ABS", 3, 6);
        
    // 8 Row    
    set_opcode(0x81, instr(instr_STA), "STA", addr(addr_mode_INDX), "INDX", 2, 6);
    set_opcode(0x84, instr(instr_STY), "STY", addr(addr_mode_ZP), "ZP", 2, 3);
    set_opcode(0x85, instr(instr_STA), "STA", addr(addr_mode_ZP), "ZP", 2, 3);
    set_opcode(0x86, instr(instr_STX), "STX", addr(addr_mode_ZP), "ZP", 2, 3);
    set_opcode(0x88, instr(instr_DEY), "DEY", addr(addr_mode_IMP), "IMP", 1, 2);
    set_opcode(0x8A, instr(instr_TXA), "TXA", addr(addr_mode_IMP), "IMP", 1, 2);
    set_opcode(0x8C, instr(instr_STY), "STY", addr(addr_mode_ABS), "ABS", 3, 4);
    set_opcode(0x8D, instr(instr_STA), "STA", addr(addr_mode_ABS), "ABS", 3, 4);
    set_opcode(0x8E, instr(instr_STX), "STX", addr(addr_mode_ABS), "ABS", 3, 4);
        
    // 9 Row    
    set_opcode(0x90, instr(instr_BCC), "BCC", addr(addr_mode_REL), "REL", 2, 2);
    set_opcode(0x91, instr(instr_STA), "STA", addr(addr_mode_INDY), "INDY", 2, 6);
    set_opcode(0x94, instr(instr_STY), "STY", addr(addr_mode_ZPX), "ZPX", 2, 4);
    set_opcode(0x95, instr(instr_STA), "STA", addr(addr_mode_ZPX), "ZPX", 2, 4);
    set_opcode(0x96, instr(instr_STX), "STX", addr(addr_mode_ZPY), "ZPY", 2, 4);
    set_opcode(0x98, instr(instr_TYA), "TYA", addr(addr_mode_IMP), "IMP", 1, 2);
    set_opcode(0x99, instr(instr_STA), "STA", addr(addr_mode_ABSY), "ABSY", 3, 5);
    set_opcode(0x9A, instr(instr_TXS), "TXS", addr(addr_mode_IMP), "IMP", 1, 2);
    set_opcode(0x9D, instr(instr_STA), "STA", addr(addr_mode_ABSX), "ABSX", 3, 5);
        
    // A Row    
    set_opcode(0xA0, instr(instr_LDY), "LDY", addr(addr_mode_IMM), "IMM", 2, 2);
    set_opcode(0xA1, instr(instr_LDA), "LDA", addr(addr_mode_INDX), "INDX", 2, 6);
    set_opcode(0xA2, instr(instr_LDX), "LDX", addr(addr_mode_IMM), "IMM", 2, 2);
    set_opcode(0xA4, instr(instr_LDY), "LDY", addr(addr_mode_ZP), "ZP", 2, 3);
    set_opcode(0xA5, instr(instr_LDA), "LDA", addr(addr_mode_ZP), "ZP", 2, 3);
    set_opcode(0xA6, instr(instr_LDX), "LDX", addr(addr_mode_ZP), "ZP", 2, 3);
    set_opcode(0xA8, instr(instr_TAY), "TAY", addr(addr_mode_IMP), "IMP", 1, 2);
    set_opcode(0xA9, instr(instr_LDA), "LDA", addr(addr_mode_IMM), "IMM", 2, 2);
    set_opcode(0xAA, instr(instr_TAX), "TAX", addr(addr_mode_IMP), "IMP", 1, 2);
    set_opcode(0xAC, instr(instr_LDY), "LDY", addr(addr_mode_ABS), "ABS", 3, 4);
    set_opcode(0xAD, instr(instr_LDA), "LDA", addr(addr_mode_ABS), "ABS", 3, 4);
    set_opcode(0xAE, instr(instr_LDX), "LDX", addr(addr_mode_ABS), "ABS", 3, 4);
        
    // B Row    
    set_opcode(0xB0, instr(instr_BCS), "BCS", addr(addr_mode_REL), "REL", 2, 2);
    set_opcode(0xB1, instr(instr_LDA), "LDA", addr(addr_mode_INDY), "INDY", 2, 5);
    set_opcode(0xB4, instr(instr_LDY), "LDY", addr(addr_mode_ZPX), "ZPX", 2, 4);
    set_opcode(0xB5, instr(instr_LDA), "LDA", addr(addr_mode_ZPX), "ZPX", 2, 4);
    set_opcode(0xB6, instr(instr_LDX), "LDX", addr(addr_mode_ZPY), "ZPY", 2, 4);
    set_opcode(0xB8, instr(instr_CLV), "CLV", addr(addr_mode_IMP), "IMP", 1, 2);
    set_opcode(0xB9, instr(instr_LDA), "LDA", addr(addr_mode_ABSY), "ABSY", 3, 4);
    set_opcode(0xBA, instr(instr_TSX), "TSX", addr(addr_mode_IMP), "IMP", 1, 2);
    set_opcode(0xBC, instr(instr_LDY), "LDY", addr(addr_mode_ABSX), "ABSX", 3, 4);
    set_opcode(0xBD, instr(instr_LDA), "LDA", addr(addr_mode_ABSX), "ABSX", 3, 4);
    set_opcode(0xBE, instr(instr_LDX), "LDX", addr(addr_mode_ABSY), "ABSY", 3, 4);
        
    // C Row    
    set_opcode(0xC0, instr(instr_CPY), "CPY", addr(addr_mode_IMM), "IMM", 2, 2);
    set_opcode(0xC1, instr(instr_CMP), "CMP", addr(addr_mode_INDX), "INDX", 2, 6);
    set_opcode(0xC4, instr(instr_CPY), "CPY", addr(addr_mode_ZP), "ZP", 2, 3);
    set_opcode(0xC5, instr(instr_CMP), "CMP", addr(addr_mode_ZP), "ZP", 2, 3);
    set_opcode(0xC6, instr(instr_DEC), "DEC", addr(addr_mode_ZP), "ZP", 2, 5);
    set_opcode(0xC8, instr(instr_INY), "INY", addr(addr_mode_IMP), "IMP", 1, 2);
    set_opcode(0xC9, instr(instr_CMP), "CMP", addr(addr_mode_IMM), "IMM", 2, 2);
    set_opcode(0xCA, instr(instr_DEX), "DEX", addr(addr_mode_IMP), "IMP", 1, 2);
    set_opcode(0xCC, instr(instr_CPY), "CPY", addr(addr_mode_ABS), "ABS", 3, 4);
    set_opcode(0xCD, instr(instr_CMP), "CMP", addr(addr_mode_ABS), "ABS", 3, 4);
    set_opcode(0xCE, instr(instr_DEC), "DEC", addr(addr_mode_ABS), "ABS", 3, 6);
        
    // D Row    
    set_opcode(0xD0, instr(instr_BNE), "BNE", addr(addr_mode_REL), "REL", 2, 2);
    set_opcode(0xD1, instr(instr_CMP), "CMP", addr(addr_mode_INDY), "INDY", 2, 5);
    set_opcode(0xD5, instr(instr_CMP), "CMP", addr(addr_mode_ZPX), "ZPX", 2, 4);
    set_opcode(0xD6, instr(instr_DEC), "DEC", addr(addr_mode_ZPX), "ZPX", 2, 6);
    set_opcode(0xD8, instr(instr_CLD), "CLD", addr(addr_mode_IMP), "IMP", 1, 2);
    set_opcode(0xD9, instr(instr_CMP), "CMP", addr(addr_mode_ABSY), "ABSY", 3, 4);
    set_opcode(0xDD, instr(instr_CMP), "CMP", addr(addr_mode_ABSX), "ABSX", 3, 4);
    set_opcode(0xDE, instr(instr_DEC), "DEC", addr(addr_mode_ABSX), "ABSX", 3, 7);
        
    // E Row    
    set_opcode(0xE0, instr(instr_CPX), "CPX", addr(addr_mode_IMM), "IMM", 2, 2);
    set_opcode(0xE1, instr(instr_SBC), "SBC", addr(addr_mode_INDX), "INDX", 2, 6);
    set_opcode(0xE4, instr(instr_CPX), "CPX", addr(addr_mode_ZP), "ZP", 2, 3);
    set_opcode(0xE5, instr(instr_SBC), "SBC", addr(addr_mode_ZP), "ZP", 2, 3);
    set_opcode(0xE6, instr(instr_INC), "INC", addr(addr_mode_ZP), "ZP", 2, 5);
    set_opcode(0xE8, instr(instr_INX), "INX", addr(addr_mode_IMP), "IMP", 1, 2);
    set_opcode(0xE9, instr(instr_SBC), "SBC", addr(addr_mode_IMM), "IMM", 2, 2);
    set_opcode(0xEA, instr(instr_NOP), "NOP", addr(addr_mode_IMP), "IMP", 1, 2);
    set_opcode(0xEC, instr(instr_CPX), "CPX", addr(addr_mode_ABS), "ABS", 3, 4);
    set_opcode(0xED, instr(instr_SBC), "SBC", addr(addr_mode_ABS), "ABS", 3, 4);
    set_opcode(0xEE, instr(instr_INC), "INC", addr(addr_mode_ABS), "ABS", 3, 6);
    set_opcode(0xF0, instr(instr_BEQ), "BEQ", addr(addr_mode_REL), "REL", 2, 2);
        
    // F Row    
    set_opcode(0xF1, instr(instr_SBC), "SBC", addr(addr_mode_INDY), "INDY", 2, 5);
    set_opcode(0xF5, instr(instr_SBC), "SBC", addr(addr_mode_ZPX), "ZPX", 2, 4);
    set_opcode(0xF6, instr(instr_INC), "INC", addr(addr_mode_ZPX), "ZPX", 2, 6);
    set_opcode(0xF8, instr(instr_SED), "SED", addr(addr_mode_IMP), "IMP", 1, 2);
    set_opcode(0xF9, instr(instr_SBC), "SBC", addr(addr_mode_ABSY), "ABSY", 3, 4);
    set_opcode(0xFD, instr(instr_SBC), "SBC", addr(addr_mode_ABSX), "ABSX", 3, 4);
    set_opcode(0xFE, instr(instr_INC), "INC", addr(addr_mode_ABSX), "ABSX", 3, 7);

}   

void cpu::set_opcode(uint16_t index, std::function<uint8_t(void)> instruction, std::string name, std::function<uint8_t(void)> address_mode, std::string address_mode_name, uint8_t instruction_bytes, uint8_t cycles_needed) {
    if (index < OPCODE_COUNT) {
        _opcode_decoder_lookup[index].name = name;
        _opcode_decoder_lookup[index].instruction = instruction;
        _opcode_decoder_lookup[index].address_mode = address_mode;
        _opcode_decoder_lookup[index].address_mode_name = address_mode_name;
        _opcode_decoder_lookup[index].instruction_bytes = instruction_bytes;
        _opcode_decoder_lookup[index].cycles_needed = cycles_needed;
    }
}
