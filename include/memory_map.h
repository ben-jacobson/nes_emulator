#pragma once
#include <stdint.h>

/*
    CPU memory map
*/ 

// main 2Kb of RAM
constexpr uint16_t RAM_ADDRESS_SPACE_START      = 0x0000;
constexpr uint16_t RAM_ADDRESS_SPACE_END        = 0x1FFF;       // there are four mirrors of the RAM from 0x0000 through to 0x07FF. 
constexpr uint16_t RAM_SIZE_BYTES               = 0x0800;       // 2048 or 2Kb for RAM buffer
// Ram runs between 0x0000 and 0x07FF, with three mirrors, first 0x0800-0x0FFF, second 0x1000-0x17FF, and third 0x1800-0x1FFF

// APU and I/O registers
constexpr uint16_t APUIO_ADDRESS_SPACE_START    = 0x4000;
constexpr uint16_t APUIO_ADDRESS_SPACE_END      = 0x4017;

// Some additional APU and IO, however these are disabled
constexpr uint16_t APU_DISABLED_SPACE_START     = 0x4018;
constexpr uint16_t APU_DISABLED_SPACE_END       = 0x401F;

// ROM Cartridge. Program ROM, Program RAM and mapper registers
constexpr uint16_t CART_ADDRESS_SPACE_START     = 0x4020;
constexpr uint16_t CART_ADDRESS_SPACE_END       = 0xFFFF;
constexpr uint16_t CART_SIZE_BYTES              = 0xBFE0;       // 49120 bytes or 47Kb total size

// Part of the RAM address space have special functions

// Zero page of RAM
constexpr uint16_t ZERO_PAGE_START              = 0x0000;
constexpr uint16_t ZERO_PAGE_END                = 0x00FF;

// Stack page of RAM
constexpr uint16_t STACK_START                  = 0x0100;
constexpr uint16_t STACK_END                    = 0x01FF;
constexpr uint8_t STACK_SIZE_BYTES              = 0xFF;         // 255 bytes

// Additional special use cases for various mappers. E.g this is often used for PRG_RAM, but is sometimes this is used for battery saves
constexpr uint16_t PRG_RAM_START                = 0x6000;
constexpr uint16_t PRG_RAM_END                  = 0x7FFF;

// Some areas of the cartridge address space have special purposes

// Usual ROM space, including mapper registers
constexpr uint16_t PGM_ROM_ADDRESS_SPACE_START      = 0x8000;
constexpr uint16_t PGM_ROM_ADDRESS_SPACE_END        = 0xFFFF;
constexpr uint16_t PGM_ROM_SIZE_BYTES               = 0x8000;       // 32,768 bytes or 32Kb for ROM. This is a default value and the cartridge will be resized on load.

// Non maskable interrupt vector
constexpr uint16_t NMI_VECTOR_START             = 0xFFFA; 
constexpr uint16_t NMI_VECTOR_END               = 0xFFFB;

// Reset vector, this area of the cartridge is the start of the program
constexpr uint16_t RESET_VECTOR_LOW             = 0xFFFC; 
constexpr uint16_t RESET_VECTOR_HIGH            = 0xFFFD;

// IRQ and BRK vectors
constexpr uint16_t IRQBRK_VECTOR_START          = 0xFFFE; 
constexpr uint16_t IRQBRK_VECTOR_END            = 0xFFFF;

// Picture processing unit
constexpr uint16_t PPU_ADDRESS_SPACE_START      = 0x2000;
constexpr uint16_t PPU_ADDRESS_SPACE_END        = 0x2007;

// PPU Mirrors (repeating every 8 bytes)
constexpr uint16_t PPU_MIRROR_SPACE_START       = 0x2008;
constexpr uint16_t PPU_MIRROR_SPACE_END         = 0x3FFF;

/*
    PPU memory map - The PPU addresses a 16kB space, $0000-3FFF, completely separate from the CPU
*/ 

// Pattern tables
constexpr uint16_t PATTERN_TABLE_0_START        = 0x0000;
constexpr uint16_t PATTERN_TABLE_0_END          = 0x0FFF;

constexpr uint16_t PATTERN_TABLE_1_START        = 0x1000;
constexpr uint16_t PATTERN_TABLE_1_END          = 0x1FFF;

constexpr uint16_t PATTERN_TABLE_SIZE           = 0x1000;   // Each pattern table is 4Kb in size

// Name tables
constexpr uint16_t NAMETABLE_0_START            = 0x2000;
constexpr uint16_t NAMETABLE_0_END              = 0x23FF;

constexpr uint16_t NAMETABLE_1_START            = 0x2400;
constexpr uint16_t NAMETABLE_1_END              = 0x27FF;

constexpr uint16_t NAMETABLE_2_START            = 0x2800;
constexpr uint16_t NAMETABLE_2_END              = 0x2BFF;

constexpr uint16_t NAMETABLE_3_START            = 0x2C00;
constexpr uint16_t NAMETABLE_3_END              = 0x2FFF;

constexpr uint16_t NAMETABLE_SIZE               = 0x0400;       // Each name table is 1Kb

// Another word for nametable is VRAM
constexpr uint16_t VRAM_START                   = NAMETABLE_0_START;
constexpr uint16_t VRAM_END                     = NAMETABLE_3_END;

// Nametable 0 and 1 mirror
constexpr uint16_t NT_MIRROR_START              = 0x3000;
constexpr uint16_t NT_MIRROR_END                = 0x3EFF; // shoudln't this be 33FF? hmm...

// Palette RAM indexes
constexpr uint16_t PALETTE_RAM_INDEX_START      = 0x3F00; 
constexpr uint16_t PALETTE_RAM_INDEX_END        = 0x3F1F; 

constexpr uint16_t PALETTE_RAM_MIRROR_START     = 0x3F20; 
constexpr uint16_t PALETTE_RAM_MIRROR_END       = 0x3FFF;

constexpr uint16_t PALETTE_RAM_SIZE             = 0x0020; // each pallette ram is 32 bytes long 

// CHR ROM on the cartridge is typically mapped to the same location as Pattern Tables 0 & 1
constexpr uint16_t CHR_ROM_START                = 0x0000;
constexpr uint16_t CHR_ROM_END                  = 0x1FFF;
constexpr uint16_t CHR_ROM_SIZE_BYTES           = 0x2000;       // 8Kb default chr rom size
