#pragma once
#include <functional>

#include "DataTypes.h"

namespace auvc {

class Consumer {
public:
    virtual ~Consumer() = default;
    virtual void consume(const Frame& frame) = 0;
    virtual void stopConsuming() = 0;
};

class ConsumerToFrameCallback : public Consumer {
public:
    using FrameCallback = std::function<void(const auvc::Frame &frame)>;
private:
    FrameCallback frameCallback;
public:
    explicit ConsumerToFrameCallback(FrameCallback callback) : frameCallback(std::move(callback)) {}
    void consume(const Frame& frame) override {
        if (frameCallback) {
            frameCallback(frame);
        }
    }
    void stopConsuming() override {
        // No specific action needed for frame callback
    }
};

} // namespace auvc