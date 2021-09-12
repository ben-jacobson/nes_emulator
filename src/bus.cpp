#include "bus.h"

bus::bus() {
    _device_index = 0;
}

void bus::set_address(uint16_t address) {
    _address = address;
}

uint16_t bus::read_address(void) {
    return _address;
}

void bus::write_data(uint8_t data) {
    int device_index = get_index_of_connected_device(_address);
    
    if (device_index != -1 && devices_connected_to_bus[device_index]._device_ptr != nullptr) {
        if (devices_connected_to_bus[device_index]._write_function_ptr == nullptr) {
            devices_connected_to_bus[device_index]._device_ptr->write(_address, data);
        }
        else {
            devices_connected_to_bus[device_index]._device_ptr->write(_address, data);
        }
    }
}

uint8_t bus::read_data(void) {
    int device_index = get_index_of_connected_device(_address);

    if (device_index != -1 && devices_connected_to_bus[device_index]._device_ptr != nullptr) {
        // our preference is to call the function pointer via it's device pointer so as to bypass the overhead from std::function. 
        // I know this is a contentious issue, but this makes a noticable difference to our frame rate, we call these pointers tens of thousands of times per frame
        // nano second savings add up to substaintial framerate improvements        
        if (devices_connected_to_bus[device_index]._read_function_ptr == nullptr) {
            _data = devices_connected_to_bus[device_index]._device_ptr->read(_address);
        }
        else {
            _data = devices_connected_to_bus[device_index]._read_function_ptr(_address);             
        }
        return _data;
    }
    return 0; 
}

uint8_t bus::debug_read_data(uint16_t address) {
    // read data from the bus without the need to alter the address, used for debugging displays where it's important not to interfere with the address, e.g reading from the PPU which increments VRAM address
    int device_index = get_index_of_connected_device(address);

    if (device_index != -1 && devices_connected_to_bus[device_index]._device_ptr != nullptr) {
        if (devices_connected_to_bus[device_index]._read_function_ptr == nullptr) {
            _data = devices_connected_to_bus[device_index]._device_ptr->read(address);
        }
        else {
            _data = devices_connected_to_bus[device_index]._read_function_ptr(address);             
        }
        return _data;
    }
    return 0;     
}


void bus::register_new_bus_device(uint16_t address_range_start, uint16_t address_range_end, bus_device* device_ptr, std::function<uint8_t(uint16_t)> read_function_ptr, std::function<void(uint16_t, uint8_t)> write_function_ptr) {
    if (_device_index < MAX_BUS_DEVICES) {
        devices_connected_to_bus[_device_index]._address_range_start = address_range_start;
        devices_connected_to_bus[_device_index]._address_range_end = address_range_end;
        devices_connected_to_bus[_device_index]._device_ptr = device_ptr;
        devices_connected_to_bus[_device_index]._read_function_ptr = read_function_ptr;
        devices_connected_to_bus[_device_index]._write_function_ptr = write_function_ptr;                
        _device_index++;
    }
    else {
        std::cout << "Error: attempting to register new bus device, but max devices reached." << std::endl;
    }
}

void bus::clear_bus_devices(void) {    
    for (uint8_t i = 0; i < MAX_BUS_DEVICES; i++) {
        // clear the struct (we could probably just get away with device index resetting back to zero but that's fine)
        std::memset(devices_connected_to_bus, 0, sizeof(devices_connected_to_bus));
    }
    _device_index = 0;
}

int bus::get_index_of_connected_device(uint16_t address) {
    for (uint8_t i = 0; i < MAX_BUS_DEVICES; i++) {
        if (address >= devices_connected_to_bus[i]._address_range_start && address <= devices_connected_to_bus[i]._address_range_end) {
            return i;
        }
    }
    return -1;
}
