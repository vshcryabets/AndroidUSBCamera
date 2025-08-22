#include "DataTypes.h"

namespace auvc {

Frame& Frame::operator=(const Frame& other) {
    if (this == &other) {
        return *this;
    }
    // Deep copy the data
    if (size == other.size) {
        width = other.width;
        height = other.height;
        format = other.format;
        if (getData() && other.getData()) {
            std::copy(other.getData(), other.getData() + other.size, getData());
        }
        timestamp = other.timestamp;
    } else {
        throw std::runtime_error("Frame assignment operator: size mismatch or null data");
    }

    return *this;
}

OwnBufferFrame& OwnBufferFrame::operator=(const Frame& other)
{
    if (this == &other) {
        return *this;
    }
    if (ownData == nullptr || getSize() != other.getSize()) {
        ownData = std::make_unique<uint8_t[]>(other.getSize());
        size = other.getSize();
    }
    Frame::operator=(other);
    return *this;
}

OwnBufferFrame& OwnBufferFrame::operator=(const OwnBufferFrame& other)
{
    if (this == &other) {
        return *this;
    }
    return *this = static_cast<const Frame&>(other);
}

} // namespace auvc