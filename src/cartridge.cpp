#include "cartridge.h"

cartridge::cartridge(uint16_t address_space_lower, uint16_t address_space_upper) 
:   bus_device()
{
    // disable the write function pointer, making this read only
    _write_function_ptr = nullptr;

    // temporarily set the address space boundaries to zero in this implementation
    _address_space_lower = address_space_lower;
    _address_space_upper = address_space_upper;   

    // set the ppu read and write function pointers as placeholders
	_ppu_read_function_ptr = std::bind(&cartridge::ppu_read, this, std::placeholders::_1);
    //_ppu_write_function_ptr = std::bind(&cartridge::ppu_write, this, std::placeholders::_1, std::placeholders::_2);    // not deemed necessary right now. 

    // start with an initial size, these will be resized later
    _pgm_rom_data.resize(PGM_ROM_SIZE_BYTES);          
    _chr_rom_data.resize(CHR_ROM_SIZE_BYTES);    

    // For our unit tests to pass, we need the cartridge to start in a default state. The cartridge load function will overwrite all of this. But essentially start in a mapper zero state
    mapper_00* placeholder_mapper_00 = new mapper_00(false, _pgm_rom_data.size(), _chr_rom_data.size());
    _mapper = placeholder_mapper_00;

}

void cartridge::load_content_from_stream(std::string bytecode, uint16_t destination_address) {      // allows for quick overwriting of character rom
    std::istringstream tokenizer(bytecode);
    std::string token;
    uint16_t index = destination_address - (PGM_ROM_ADDRESS_SPACE_END - _pgm_rom_data.size() + 1); // allow mirroring if needs be

    while (std::getline(tokenizer, token, ' ') && index < PGM_ROM_ADDRESS_SPACE_END) {
        _pgm_rom_data[index] = (uint8_t)strtol(token.c_str(), NULL, 16);
        index++;
    } 
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

    /* 
    From here we will read the header info from the cartridge and set everything up as required

    Complete definition can be found here: https://wiki.nesdev.com/w/index.php?title=NES_2.0

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

    */ 

    // check the file format from the header, our emulator is fully compatible with iNes format, but also has limited NES2.0 support
    bool iNESFormat = false;
    if (file_contents[0] == 'N' && file_contents[1] == 'E' && file_contents[2] == 'S' && file_contents[3] == 0x1A)
        iNESFormat = true;

    bool NES20Format = false;
    if (iNESFormat == true && (file_contents[7] & 0x0C) == 0x08)
        NES20Format = true;    

    std::cout << "iNES format compatible: " << (iNESFormat ? "True" : "False") << std::endl;
    std::cout << "NES 2.0 format compatible: " << (NES20Format ? "True" : "False") << std::endl;

    uint8_t pgm_rom_size_lsb = file_contents[4];
    uint8_t chr_rom_size_lsb = file_contents[5];
    
    uint8_t flags = file_contents[6];
    bool nametable_vertical_mirroring = (flags & 1) == 1 ? true : false;
    bool battery_backed_ram = ((flags & (1 << 1)) >> 1) == 1 ? true : false;
    bool trainer_present = ((flags & (1 << 2)) >> 2) == 1 ? true : false;
    uint8_t mapper_number = flags >> 4;

    std::cout << "Flags: " << (uint16_t)flags << std::endl;
    std::cout << "    Nametable Mirroring: " << (nametable_vertical_mirroring ? "Vertical" : "Horizontal") << std::endl;
    std::cout << "    Cartridge contains battery-backed PRG RAM ($6000-7FFF) or other persistent memory: " << (battery_backed_ram ? "True" : "False") << std::endl;
    std::cout << "    512-byte trainer at $7000-$71FF: " << (trainer_present ? "True" : "False") << std::endl;
    std::cout << "    Ignore mirroring control or above mirroring bit; instead provide four-screen VRAM: " << ((flags & (1 << 3) >> 3) ? "True" : "False") << std::endl;
    std::cout << "    Mapper: " << (uint16_t)mapper_number << std::endl;

   /*
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
    */ 

    uint8_t pgm_rom_size = pgm_rom_size_lsb & 0x0F;
    uint8_t chr_rom_size = chr_rom_size_lsb & 0x0F;

    if (NES20Format) {
        // define our additional flags needed
        uint8_t extended_flags = file_contents[7];
        uint8_t console_type = extended_flags & 0x03; // just the two lowest bits

        std::cout << "Nes 2.0 Extended flags: " << std::endl;
        std::cout <<  "    Console type: ";

        switch (console_type) {
            case 0:
                std::cout << "Nintendo Entertainment System/Family Computer" << std::endl;
                break;
            case 1:
                std::cout << "Nintendo Vs. System" << std::endl;
                break;
            case 2:
                std::cout << "Nintendo Playchoice 10" << std::endl;
                break;
            case 3: 
                std::cout << "Extended Console Type" << std::endl;
                break;
        }

        uint8_t extended_mapper = (extended_flags & 0xF0);
        mapper_number |= extended_mapper;
        std::cout << "Final mapper: " << mapper_number << std::endl;

        //uint8_t sub_mappers = file_contents[8]; // our emulation does not implement any sub mappers yet

        // combine the upper bits to allow for extended rom sizes, mappers, etc
        uint8_t extended_pgm_rom_size = (file_contents[9] & 0x0F) << 4; 
        pgm_rom_size |= extended_pgm_rom_size;

        uint8_t extended_chr_rom_size = file_contents[9] & 0xF0; // just the upper 8 bits
        chr_rom_size |= extended_chr_rom_size;
    }

    std::cout << "PRG_ROM_SIZE: " << (pgm_rom_size * 16) << "kb" << std::endl;
    _pgm_rom_data.resize(pgm_rom_size * 16 * 1024);     

    std::cout << "CHR_ROM_SIZE: " << (chr_rom_size * 8) << "kb" << std::endl;
    _chr_rom_data.resize(chr_rom_size * 8 * 1024);

    // todo - assign the mapper as per what's in the file
    _mapper->set_pgm_rom_size(_pgm_rom_data.size());
    _mapper->set_chr_rom_size(_chr_rom_data.size());
    _mapper->set_pgm_rom_mirroring(false);

    /* // the rest of these features are yet to be implemeted, for the time being we just want to get Mapper 0 getting up and running, and will look at the rest later
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

    uint16_t file_contents_read_address = 16;    // after the 16 byte header from 0-15

    // skip the trainer area for now (if present)
    if (trainer_present) {
        std::cout << "Trainer found. Skipping first 512 bytes" << std::endl;
        file_contents_read_address += 512;   // skip the 512 bytes
    }

    std::cout << "Reading PRG ROM from file offset: " << file_contents_read_address << std::endl;

    // load the PGM ROM data into the buffer that we recently resized. 
    for (uint16_t i = 0; i < (pgm_rom_size * 16 * 1024); i++) {
        _pgm_rom_data[i] = file_contents[i + file_contents_read_address];   // two copies mirrored back to back
    }

    // update the offset
    file_contents_read_address += pgm_rom_size * 16 * 1024;
    std::cout << "Reading CHR ROM from file offset: " << file_contents_read_address << std::endl;

    // load the CHR ROM data into the buffer
    for (uint16_t i = 0; i < (chr_rom_size * 8 * 1024); i++) {
        _chr_rom_data[i] = file_contents[i + file_contents_read_address];    
    }

    delete[] file_contents;
    return true;
}

uint8_t cartridge::read(uint16_t address) {
    if (address >= _address_space_lower && address <= _address_space_upper) {
        int mapped_index = _mapper->cpu_read_address(address); 

        if (mapped_index != -1 && mapped_index < (int)_pgm_rom_data.size()) {
            return _pgm_rom_data[(uint16_t)mapped_index];       // for type safety
        }
    }
    return 0;
}

void cartridge::write(uint16_t address, uint8_t data) {
    if (address >= _address_space_lower && address <= _address_space_upper) {
        _mapper->cpu_write_address(address, data); 
    }
}	

uint8_t cartridge::ppu_read(uint16_t address) {
    if (/*address >= CHR_ROM_START && */address <= CHR_ROM_END) {
        int mapped_index = _mapper->ppu_read_address(address); 

        if (mapped_index != -1 && mapped_index < (int)_chr_rom_data.size()) {
            return _chr_rom_data[mapped_index % CHR_ROM_SIZE_BYTES];
        }
    }
    return 0;
}

void cartridge::ppu_write(uint16_t address, uint8_t data) {
    if (/*address >= CHR_ROM_START && */address <= CHR_ROM_END) {
        _mapper->ppu_write_adress(address, data); 
    }
}

/*uint8_t cartridge::read_rom(uint16_t address) {
    // First check if the read is within the specified address range
    if (address >= PGM_ROM_ADDRESS_SPACE_START) { //   && address <= ROM_ADDRESS_SPACE_END) {  // we won't check that is at end address because it it's the largest value a uint16_t can have
        return _pgm_rom_data[address - PGM_ROM_ADDRESS_SPACE_START]; // new mapped address is offset by _address_space_lower;
    }
    return 0;
}*/

uint8_t cartridge::debug_read(uint16_t relative_address) {
    // notice there is no address space checking, we simply output whatever is at the relative address, e.g 0 is the start and MAX_SIZE is the end
    if (relative_address < _pgm_rom_data.size()) {
        return _pgm_rom_data[relative_address];   
    } 
    else {
        return 0;
    }
}

void cartridge::debug_write_relative(uint16_t relative_address, uint8_t data) {
    if (relative_address <= _pgm_rom_data.size()) {
        _pgm_rom_data[relative_address] = data;
    }
}

void cartridge::debug_write_absolute(uint16_t absolute_address, uint8_t data) {
    if (absolute_address >= PGM_ROM_ADDRESS_SPACE_START) { // && address <= PGM_ROM_ADDRESS_SPACE_END) { // removing for warnings
        uint16_t index = absolute_address - (PGM_ROM_ADDRESS_SPACE_END - _pgm_rom_data.size() + 1);  // PGM rom always sits at the end of the memory map
        
        if (index <= _pgm_rom_data.size()) {
            if (_mapper->get_pgm_rom_mirroring()) {
                index = index % (PGM_ROM_SIZE_BYTES / 2);     // repeat every 16Kb for the 32Kb roms
            }
            _pgm_rom_data[index] = data;
        }
    }    
}