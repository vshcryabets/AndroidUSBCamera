#pragma once
#include <cstdint>
#include <vector>

struct CameraResolution {
    const uint8_t id;
    const uint8_t subtype;
    const uint8_t frameIndex;
    const uint16_t width;
    const uint16_t height;
    std::vector<uint32_t> frameIntervals; // in 100ns units
};

class Camera {
public:
    virtual ~Camera() = default;
    virtual std::vector<CameraResolution> getSupportedSize() = 0;
};
