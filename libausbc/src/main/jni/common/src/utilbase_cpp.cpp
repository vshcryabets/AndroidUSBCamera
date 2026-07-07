#include "utilbase_cpp.h"

#include <iomanip>
#include <sstream>
#include <iostream>
#include <algorithm>

namespace auvc
{
    const std::string convertToHexStringWithOffsets(const uint8_t *data, const uint16_t size)
    {
        if (size == 0)
        {
            return "0 bytes string";
        }
        if (data == nullptr)
        {
            return "null data";
        }
        constexpr size_t bytes_per_line = 16;
        std::stringstream ofstream;
        size_t lines = (size + bytes_per_line - 1) / bytes_per_line;
        for (size_t line = 0; line < lines; line++)
        {
            size_t line_start = line * bytes_per_line;
            size_t line_end = std::min(line_start + bytes_per_line, (size_t)size);
            ofstream << std::setw(4) << std::setfill('0') << std::hex << line_start << ": ";
            for (size_t i = line_start; i < line_end; i++)
            {
                ofstream << std::hex << std::setw(2) << std::setfill('0')
                         << (int)(unsigned char)data[i] << " ";
            }
            ofstream << std::dec << " ";
            for (size_t i = line_start; i < line_end; i++)
            {
                char c = (data[i] >= 32 && data[i] <= 126) ? (char)data[i] : '.';
                ofstream << c;
            }
            ofstream << std::endl;
        }
        return ofstream.str();
    }
}