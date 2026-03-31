#pragma once

#include <cstdint>
#include <string>

namespace auvc {
    const std::string convertToHexStringWithOffsets(const uint8_t* data, const uint16_t size);
}