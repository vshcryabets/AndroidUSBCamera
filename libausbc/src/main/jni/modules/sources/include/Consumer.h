#pragma once

#include <functional>
#include <string>
#include <exception>

#include "DataTypes.h"
#include "Source.h"

namespace auvc {

class OpenCloseConsumer: public Consumer {
public:
    virtual ~OpenCloseConsumer() = default;
    virtual auvc::ConsumerError startConsuming();
    virtual auvc::ConsumerError stopConsuming();
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
};

} // namespace auvc