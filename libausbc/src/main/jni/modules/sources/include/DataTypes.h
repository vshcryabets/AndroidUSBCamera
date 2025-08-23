#pragma once
#include <cstdint>
#include <cstddef>
#include <chrono>

namespace auvc {

enum FrameFormat {
        YUYV,
        RGBA,
        RGB,
        RGBX,
        YUV420P,
        ENCODED,
        NONE
    };

class Frame {
private:
    uint16_t width;
    uint16_t height;
    FrameFormat format;
    uint8_t* data;
    std::chrono::high_resolution_clock::time_point timestamp;
protected:
    size_t size;
public:

    Frame(uint16_t width, 
        uint16_t height, 
        FrameFormat format,
        uint8_t* data,
        size_t size,
        std::chrono::high_resolution_clock::time_point timestamp
    )
        : width(width),
        height(height),
        format(format),
        data(data),
        size(size),
        timestamp(timestamp) {}

    virtual ~Frame() = default;

    Frame& operator=(const Frame& other);
    [[nodiscard]] uint16_t getWidth() const { return width; }
    [[nodiscard]] uint16_t getHeight() const { return height; }
    [[nodiscard]] FrameFormat getFormat() const { return format; }
    [[nodiscard]] virtual uint8_t* getData() const { return data; }
    [[nodiscard]] size_t getSize() const { return size; }
    [[nodiscard]] std::chrono::high_resolution_clock::time_point getTimestamp() const { return timestamp; }
};

/**
 * Frame with its own buffer for image data.
 */
class OwnBufferFrame: public Frame {
private:    
    std::unique_ptr<uint8_t[]> ownData;
public:
    OwnBufferFrame(uint16_t width, 
        uint16_t height, 
        FrameFormat format,
        size_t size,
        std::chrono::high_resolution_clock::time_point timestamp
    ): Frame(width, height, format, nullptr, size, timestamp),
          ownData(new uint8_t[size])
    {
    }

    virtual ~OwnBufferFrame() = default;
    OwnBufferFrame& operator=(const Frame& other);
    OwnBufferFrame& operator=(const OwnBufferFrame& other);

    [[nodiscard]] uint8_t* getData() const override { return ownData.get(); }
};

} // namespace auvc