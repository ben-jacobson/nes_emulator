#include "cartridge.h"

cartridge::cartridge(uint16_t address_space_lower, uint16_t address_space_upper) 
:   bus_device()
{
    // disable the write function pointer
    _write_function_ptr = nullptr;

    // temporarily set the address space boundaries to zero in this implementation
    _address_space_lower = address_space_lower;
    _address_space_upper = address_space_upper;   

    _pgm_rom_data = new uint8_t[PGM_ROM_SIZE_BYTES]; // allocate the ROM within heap memory to a specified size
    // I'm happy to just use a C style array, the needs of this are really simple. 

    // initialize the rom content to all zeros
    for (uint16_t i = 0; i < PGM_ROM_SIZE_BYTES; i++) {
        _pgm_rom_data[i] = 0; // initialize rom with all zeros
    }

    // set up the reset vector start position of 0x8000
    _pgm_rom_data[RESET_VECTOR_LOW - PGM_ROM_ADDRESS_SPACE_START] = 0x00;
    _pgm_rom_data[RESET_VECTOR_HIGH - PGM_ROM_ADDRESS_SPACE_START] = 0x80;
}

cartridge::~cartridge() { // we no longer use a heap array so this is not used any more
    delete[] _pgm_rom_data; // free the memory. 
} 

void cartridge::load_content_from_stream(std::string bytecode, uint16_t destination_address) {      // allows for quick overwriting of character rom
    std::istringstream tokenizer(bytecode);
    std::string token;
    uint16_t address = destination_address - PGM_ROM_ADDRESS_SPACE_START;

    while (std::getline(tokenizer, token, ' ') && address < PGM_ROM_ADDRESS_SPACE_END) {
        _pgm_rom_data[address] = (uint8_t)strtol(token.c_str(), NULL, 16);
        address++;
    } 
}

bool cartridge::load_content_from_file(std::string filename, uint16_t start_address) {
    FILE* file_handle = fopen(filename.c_str(), "rb");      // needs to open files in binary mode

    if (file_handle == NULL) {
        return false; 
    }

    fseek(file_handle, 0, SEEK_END);    // Determine file size
    size_t file_size = ftell(file_handle);

    char* file_contents = new char[file_size];

    rewind(file_handle);
    fread(file_contents, sizeof(char), file_size, file_handle);
    fclose(file_handle);

    for (uint16_t i = 0; i < file_size; i++) {
        _pgm_rom_data[i] = file_contents[start_address + i];

        if (i >= PGM_ROM_SIZE_BYTES) {
            break;
        }
     }

    delete[] file_contents;
    return true;
}

bool cartridge::load_rom(std::string filename) {
    FILE* file_handle = fopen(filename.c_str(), "rb");      // needs to open files in binary mode

    if (file_handle == NULL) {
        return false; 
    }

    fseek(file_handle, 0, SEEK_END);    // Determine file size
    size_t file_size = ftell(file_handle);
    char* file_contents = new char[file_size];

    rewind(file_handle);
    fread(file_contents, sizeof(char), file_size, file_handle);

    fclose(file_handle);

    // check the file format from the header, this was taken directly from the nesdev site. Our emulator has a strong preference for NES 2.0 file format
    bool iNESFormat = false;
    if (file_contents[0] == 'N' && file_contents[1] == 'E' && file_contents[2] == 'S' && file_contents[3] == 0x1A)
        iNESFormat = true;

    bool NES20Format = false;
    if (iNESFormat == true && (file_contents[7] & 0x0C) == 0x08)
        NES20Format = true;    

    std::cout << "iNESFormat: " << (iNESFormat ? "True" : "False") << std::endl;
    std::cout << "NES20Format: " << (NES20Format ? "True" : "False") << std::endl;

    uint16_t prg_rom_size = file_contents[4];
    std::cout << "PRG_ROM_SIZE: " << (prg_rom_size * 16) << " Kb" << std::endl;

    uint16_t chr_rom_size = file_contents[5];
    std::cout << "CHR_ROM_SIZE: " << (chr_rom_size * 8) << " Kb" << std::endl;

    /* Things to implement in future -- see this page: https://wiki.nesdev.com/w/index.php?title=NES_2.0
    Offset Meaning
    --------------
    0-3    Identification String. Must be "NES<EOF>"., (see above)

    4      PRG-ROM size LSB
    5      CHR-ROM size LSB

    6      Flags 6
        D~7654 3210
            ---------
            NNNN FTBM
            |||| |||+-- Hard-wired nametable mirroring type
            |||| |||     0: Horizontal or mapper-controlled
            |||| |||     1: Vertical
            |||| ||+--- "Battery" and other non-volatile memory
            |||| ||      0: Not present
            |||| ||      1: Present
            |||| |+--- 512-byte Trainer
            |||| |      0: Not present
            |||| |      1: Present between Header and PRG-ROM data
            |||| +---- Hard-wired four-screen mode
            ||||        0: No
            ||||        1: Yes
            ++++------ Mapper Number D0..D3

    7      Flags 7
        D~7654 3210
            ---------
            NNNN 10TT
            |||| ||++- Console type
            |||| ||     0: Nintendo Entertainment System/Family Computer
            |||| ||     1: Nintendo Vs. System
            |||| ||     2: Nintendo Playchoice 10
            |||| ||     3: Extended Console Type
            |||| ++--- NES 2.0 identifier
            ++++------ Mapper Number D4..D7

    8      Mapper MSB/Submapper
        D~7654 3210
            ---------
            SSSS NNNN
            |||| ++++- Mapper number D8..D11
            ++++------ Submapper number

    9      PRG-ROM/CHR-ROM size MSB
        D~7654 3210
            ---------
            CCCC PPPP
            |||| ++++- PRG-ROM size MSB
            ++++------ CHR-ROM size MSB

    10     PRG-RAM/EEPROM size
        D~7654 3210
            ---------
            pppp PPPP
            |||| ++++- PRG-RAM (volatile) shift count
            ++++------ PRG-NVRAM/EEPROM (non-volatile) shift count
        If the shift count is zero, there is no PRG-(NV)RAM.
        If the shift count is non-zero, the actual size is
        "64 << shift count" bytes, i.e. 8192 bytes for a shift count of 7.

    11     CHR-RAM size
        D~7654 3210
            ---------
            cccc CCCC
            |||| ++++- CHR-RAM size (volatile) shift count
            ++++------ CHR-NVRAM size (non-volatile) shift count
        If the shift count is zero, there is no CHR-(NV)RAM.
        If the shift count is non-zero, the actual size is
        "64 << shift count" bytes, i.e. 8192 bytes for a shift count of 7.

    12     CPU/PPU Timing
        D~7654 3210
            ---------
            .... ..VV
                    ++- CPU/PPU timing mode
                        0: RP2C02 ("NTSC NES")
                        1: RP2C07 ("Licensed PAL NES")
                        2: Multiple-region
                        3: UMC 6527P ("Dendy")

    13     When Byte 7 AND 3 =1: Vs. System Type
        D~7654 3210
            ---------
            MMMM PPPP
            |||| ++++- Vs. PPU Type
            ++++------ Vs. Hardware Type

        When Byte 7 AND 3 =3: Extended Console Type
        D~7654 3210
            ---------
            .... CCCC
                ++++- Extended Console Type

    14     Miscellaneous ROMs
        D~7654 3210
            ---------
            .... ..RR
                    ++- Number of miscellaneous ROMs present

    15     Default Expansion Device
        D~7654 3210
            ---------
            ..DD DDDD
            ++-++++- Default Expansion Device

    */

    uint16_t file_pgm_rom_contents_start_address = 0x10;    // after the 16 byte header from 0-15

    // skip the trainer area for now (if present)
    if ((file_contents[6] & (1 << 2)) == 1) {
        std::cout << "Trainer found. Skipping first 512 bytes" << std::endl;
        file_pgm_rom_contents_start_address += 512;   // skip the 512 bytes
    }

    // while we test our our emulator using nestest.nes, we want to skip all the stuff that we aren't able to handle just yet. 
    // e.g we don't have any mappers. We will simply load this ROM data into location C000 and set our debug start address accordingly.
    uint16_t pgm_rom_start_address = 0xC000 - 0x8000;

    for (uint16_t i = 0; i < 16 * 1024; i++) {
        _pgm_rom_data[i + pgm_rom_start_address] = file_contents[i + file_pgm_rom_contents_start_address];

        if ((i + pgm_rom_start_address) >= PGM_ROM_SIZE_BYTES) {
            std::cout << "Error, trying to load file contents to location of bounds. Attempted index: " << i + pgm_rom_start_address << std::endl;
            break;
        }
     }

    delete[] file_contents;
    return true;
}

uint8_t cartridge::read(uint16_t address) {
    // First check if the read is within the specified address range
    if (address >= _address_space_lower && address <= _address_space_upper) {
        if (address >= PGM_ROM_ADDRESS_SPACE_START) { //  && address <= ROM_ADDRESS_SPACE_END) { // we won't check that is at end address because it it's the largest value a uint16_t can have
            return read_rom(address);
        }
        else {
            return 0; // TODO!!
        }
    }
    return 0;
}

void cartridge::write(uint16_t address, uint8_t data) {
    // do nothing, we cannot write to ROM
    address += data; // surpress warning 
    return;
}	

uint8_t cartridge::read_rom(uint16_t address) {
    // First check if the read is within the specified address range
    if (address >= PGM_ROM_ADDRESS_SPACE_START) { //   && address <= ROM_ADDRESS_SPACE_END) {  // we won't check that is at end address because it it's the largest value a uint16_t can have
        return _pgm_rom_data[address - PGM_ROM_ADDRESS_SPACE_START]; // new mapped address is offset by _address_space_lower;
    }
    return 0;
}


uint8_t cartridge::debug_read(uint16_t relative_address) {
    // notice there is no address space checking, we simply output whatever is at the relative address, e.g 0 is the start and MAX_SIZE is the end
    return _pgm_rom_data[relative_address];    
}

uint8_t* cartridge::get_rom_pointer(void) {
    return _pgm_rom_data;
}