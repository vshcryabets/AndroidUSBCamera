#include "DataTypes.h"

namespace auvc {

Frame& Frame::operator=(const Frame& other) {
    if (this != &other) {

        // Deep copy the data
        if (data && size == other.size) {
            width = other.width;
            height = other.height;
            format = other.format;
            std::copy(other.data, other.data + other.size, data);
            // size = other.size;
            timestamp = other.timestamp;
        } else {
            throw std::runtime_error("Frame assignment operator: size mismatch or null data");
        }
    }
    return *this;
}

} // namespace auvc