#include "TAG.h"

std::string hexToBytes(const std::string& hexStr) {
    if (hexStr.length() != 32) {
        throw std::invalid_argument("Input must be 32 characters long.");
    }

    std::string bytes;
    bytes.reserve(16);

    for (size_t i = 0; i < 32; i += 2) {
        std::string byteStr = hexStr.substr(i, 2);
        uint8_t byte = static_cast<uint8_t>(std::stoi(byteStr, nullptr, 16));
        bytes.push_back(static_cast<char>(byte));
    }

    return bytes;
}

std::string getUpperHalf(std::string ID){
    return ID.substr(0,ID.length()/2);
}

std::string getLowerHalf(std::string ID){
    return ID.substr(ID.length()/2,ID.length()/2);
}
