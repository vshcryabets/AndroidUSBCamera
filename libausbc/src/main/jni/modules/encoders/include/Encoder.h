#pragma once
#include <cstdint>
#include <vector>
#include <exception>
#include <string>
#include "Consumer.h"

class EncoderException : public std::exception
{
public:
    enum Type
    {
        Unknown = 0,
        NotInitialized,
        FailedToEncodeFrame,
    };

public:
    EncoderException(Type type, const std::string &message) : msg(message), type(type) {}
    EncoderException(Type type, const char *message) : msg(message), type(type) {}
    const char *what() const noexcept override
    {
        return msg.c_str();
    }
    const Type getType() const noexcept
    {
        return type;
    }

private:
    const std::string msg;
    Type type;
};

struct EncoderBuffer
{
    uint8_t *data;
    size_t size;
};

struct EncoderMultiBuffer
{
    std::vector<EncoderBuffer> buffers;
    EncoderBuffer single;
    size_t totalSize{0};
};

class Encoder: auvc::Consumer
{
public:
    Encoder() = default;
    virtual ~Encoder() {};
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void consume(const auvc::Frame& frame) override = 0;
    // virtual EncoderMultiBuffer encodeFrame() = 0;
    // virtual EncoderMultiBuffer flush() = 0;
};

struct EncoderBaseConfiguration
{
    uint32_t width;
    uint32_t height;
    uint16_t fps_num;
    uint16_t fps_den;
};

template <typename T>
class EncoderWithConfiguration
{
private:
    T config;

public:
    EncoderWithConfiguration() = default;
    virtual void open(const T &config)
    {
        this->config = config;
    }
    virtual const T &getConfiguration() const
    {
        return config;
    }
    virtual void close() = 0;
    virtual ~EncoderWithConfiguration() {}
};