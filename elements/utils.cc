//
// Created by Stan Schepers on 02/11/2019.
//

#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include "IGMPMessage.hh"

#include "utils.hh"

// thanks to https://stackoverflow.com/questions/55455591/hex-string-to-uint8-t-msg
std::vector<uint8_t> HexToBytes(const string& hex) {
    std::vector<uint8_t> bytes;
    for (unsigned int i = 0; i < hex.length(); i += 2) {
        string byteString = hex.substr(i, 2);
        uint8_t byte = (uint8_t) strtol(byteString.c_str(), nullptr, 16);
        bytes.push_back(byte);
    }
    return bytes;
}
