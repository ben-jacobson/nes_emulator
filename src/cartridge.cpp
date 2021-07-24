#include "cartridge.h"

cartridge::cartridge(bus* bus_ptr, uint16_t address_space_lower, uint16_t address_space_upper) 
:   bus_device(bus_ptr)
{
    // disable the write function pointer
    _write_function_ptr = nullptr;

    // temporarily set the address space boundaries to zero in this implementation
    _address_space_lower = address_space_lower;
    _address_space_upper = address_space_upper;   

    _rom_data = new uint8_t[ROM_SIZE_BYTES]; // allocate the ROM within heap memory to a specified size
    // I'm happy to just use a C style array, the needs of this are really simple. 

    // initialize the rom content to all zeros
    for (uint16_t i = 0; i < ROM_SIZE_BYTES; i++) {
        _rom_data[i] = 0; // initialize rom with all zeros
    }

    // set up the reset vector start position of 0x8000
    _rom_data[RESET_VECTOR_LOW - ROM_ADDRESS_SPACE_START] = 0x00;
    _rom_data[RESET_VECTOR_HIGH - ROM_ADDRESS_SPACE_START] = 0x80;
}

cartridge::~cartridge() {
    delete[] _rom_data; // free the memory. 
}

void cartridge::load_content_from_stream(std::string bytecode, uint16_t destination_address) {      // allows for quick overwriting of character rom
    std::istringstream tokenizer(bytecode);
    std::string token;
    uint16_t address = destination_address;

    while (std::getline(tokenizer, token, ' ') && address < ROM_ADDRESS_SPACE_END) {
        _rom_data[address] = (uint8_t)strtol(token.c_str(), NULL, 16);
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

    for (uint16_t i = 0; i < file_size; i++) {
        _rom_data[i] = file_contents[start_address + i];

        if (i >= ROM_SIZE_BYTES) {
            break;
        }
     }

    delete[] file_contents;
    return true;
}

bool cartridge::load_rom(std::string filename) {
    return load_content_from_file(filename, 0);
}

uint8_t cartridge::read(uint16_t address) {
    // First check if the read is within the specified address range
    if (address >= _address_space_lower && address <= _address_space_upper) {
        if (address >= ROM_ADDRESS_SPACE_START) { //  && address <= ROM_ADDRESS_SPACE_END) { // we won't check that is at end address because it it's the largest value a uint16_t can have
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
    if (address >= ROM_ADDRESS_SPACE_START) { //   && address <= ROM_ADDRESS_SPACE_END) {  // we won't check that is at end address because it it's the largest value a uint16_t can have
        return _rom_data[address - ROM_ADDRESS_SPACE_START]; // new mapped address is offset by _address_space_lower;
    }
    return 0;
}


uint8_t cartridge::debug_read(uint16_t relative_address) {
    // notice there is no address space checking, we simply output whatever is at the relative address, e.g 0 is the start and MAX_SIZE is the end
    return _rom_data[relative_address];    
}

uint8_t* cartridge::get_rom_pointer(void) {
    return _rom_data;
}