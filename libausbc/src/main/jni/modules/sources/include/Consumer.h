#pragma once

#include <functional>
#include <string>
#include <exception>

#include "DataTypes.h"

namespace auvc {

enum class ConsumerErrorCode : uint16_t {
    SUCCESS = 0,
    WRONG_CONFIGURATION
};

class ConsumerError : public std::exception {
public:
private:
    ConsumerErrorCode code;
    std::string message;
public:
    ConsumerError(ConsumerErrorCode code, const std::string &message) : code(code), message(message) {}
    ~ConsumerError() override = default;
    const char* what() const noexcept override;
    ConsumerErrorCode getCode() const noexcept { return code; }

    static ConsumerError SUCCESS;
};

class Consumer {
public:
    virtual ~Consumer() = default;
    virtual void consume(const Frame& frame) = 0;
};

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