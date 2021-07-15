#include "bus.h"

bus::bus() {
    device_index = 0;
}

void bus::set_address(uint16_t address) {
    _address = address;
}

uint16_t bus::read_address(void) {
    return _address;
}

void bus::write_data(uint8_t data) {
    _data = data; // ensure written data is placed onto bus
    int device_index = get_index_of_connected_device(_address);
    
    if (device_index != -1 && devices_connected_to_bus[device_index]._write_function_ptr != nullptr) {
        devices_connected_to_bus[device_index]._write_function_ptr();
    }
}

uint8_t bus::read_data(void) {
    int device_index = get_index_of_connected_device(_address);
    
    if (device_index != -1) {
        _data = devices_connected_to_bus[device_index]._read_function_ptr();  // ensure read data is placed onto bus as well.   
        return _data;
    }
    return 0;
}

void bus::register_new_bus_device(uint16_t address_range_start, uint16_t address_range_end, read_function_pointer read_function_ptr, write_function_pointer write_function_ptr) {
    if (device_index < MAX_BUS_DEVICES) {
        devices_connected_to_bus[device_index]._address_range_start = address_range_start;
        devices_connected_to_bus[device_index]._address_range_end = address_range_end;
        devices_connected_to_bus[device_index]._read_function_ptr = read_function_ptr;
        devices_connected_to_bus[device_index]._write_function_ptr = write_function_ptr;                
        device_index++;
    }
    else {
        std::cout << "Error: attempting to register new bus device, but max devices reached" << std::endl;
    }
}

int bus::get_index_of_connected_device(uint16_t address) {
    for (uint8_t i = 0; i < MAX_BUS_DEVICES; i++) {
        if (address >= devices_connected_to_bus[i]._address_range_start && address <= devices_connected_to_bus[i]._address_range_end) {
            return i;
        }
    }
    return -1;
}
