#pragma once

#include <stdint.h>

// main 2Kb of RAM
constexpr uint16_t RAM_ADDRESS_SPACE_START      = 0x0000;
constexpr uint16_t RAM_ADDRESS_SPACE_END        = 0x07FF;

// A mirror of RAM
constexpr uint16_t RAM_MIRRORA_SPACE_START      = 0x0800;
constexpr uint16_t RAM_MIRRORA_SPACE_END        = 0x0FFF;

// Another mirror of RAM
constexpr uint16_t RAM_MIRRORB_SPACE_START      = 0x1000;
constexpr uint16_t RAM_MIRRORB_SPACE_END        = 0x17FF;

// One last mirror of RAM
constexpr uint16_t RAM_MIRRORC_SPACE_START      = 0x1800;
constexpr uint16_t RAM_MIRRORC_SPACE_END        = 0x1FFF;

// PPU
constexpr uint16_t PPU_ADDRESS_SPACE_START      = 0x2000;
constexpr uint16_t PPU_ADDRESS_SPACE_END        = 0x2007;

// PPU Mirrors (repeating every 8 bytes)
constexpr uint16_t PPU_MIRROR_SPACE_START       = 0x2008;
constexpr uint16_t PPU_MIRROR_SPACE_END         = 0x3FFF;

// APU and I/O registers
constexpr uint16_t APUIO_ADDRESS_SPACE_START    = 0x4000;
constexpr uint16_t APUIO_ADDRESS_SPACE_END      = 0x4017;

// Some additional APU and IO, however these are disabled
constexpr uint16_t APU_DISABLED_SPACE_START     = 0x4018;
constexpr uint16_t APU_DISABLED_SPACE_END       = 0x401F;

// ROM Cartridge. Program ROM, Program RAM and mapper registers
constexpr uint16_t CART_ADDRESS_SPACE_START     = 0x4020;
constexpr uint16_t CART_ADDRESS_SPACE_END       = 0xFFFF;

// Part of the RAM address space have special functions

// Zero page of RAM
constexpr uint16_t ZERO_PAGE_START              = 0x0000;
constexpr uint16_t ZERO_PAGE_END                = 0x00FF;

// Stack page of RAM
constexpr uint16_t STACK_PAGE_START             = 0x0100;
constexpr uint16_t STACK_PAGE_END               = 0x01FF;

// Additional special use cases for RAM
constexpr uint16_t BATTERY_SAVE_START           = 0x6000;
constexpr uint16_t BATTERY_SAVE_END             = 0x7FFF;

// Some areas of the cartridge address space have special purposes

// Usual ROM space, including mapper registers
constexpr uint16_t ROM_ADDRESS_SPACE_START      = 0x8000;
constexpr uint16_t ROM_ADDRESS_SPACE_END        = 0xFFFF;

// Non maskable interrupt vector
constexpr uint16_t NMI_VECTOR_START             = 0xFFFA; 
constexpr uint16_t NMI_VECTOR_END               = 0xFFFB;

// Reset vector, this area of the cartridge is the start of the program
constexpr uint16_t RESET_VECTOR_START           = 0xFFFC; 
constexpr uint16_t RESET_VECTOR_END             = 0xFFFD;

// IRQ and BRK vectors
constexpr uint16_t IRQBRK_VECTOR_START          = 0xFFFE; 
constexpr uint16_t IRQBRK_VECTOR_END            = 0xFFFF;

