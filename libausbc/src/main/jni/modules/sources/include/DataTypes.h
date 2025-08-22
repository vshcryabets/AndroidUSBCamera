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

struct Frame {
    const uint16_t width;
    const uint16_t height;
    const FrameFormat format;

    uint8_t* data {nullptr};
    size_t size;
    std::chrono::high_resolution_clock::time_point timestamp {std::chrono::high_resolution_clock::now()};

    Frame(uint16_t width, uint16_t height, FrameFormat format)
        : width(width),
        height(height),
        format(format),
        data(nullptr),
        size(0) {}
};

} // namespace auvc