#pragma once
#include <cstdint>
#include <vector>
#include <exception>
#include <string>

class DecoderException : public std::exception
{
public:
    enum Type
    {
        Unknown = 0,
        NotInitialized,
        FailedToDecodeFrame,
    };

public:
    DecoderException(Type type, const std::string &message) : msg(message), type(type) {}
    DecoderException(Type type, const char *message) : msg(message), type(type) {}
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

struct DecoderBuffer
{
    uint8_t *data;
    size_t size;
};

struct DecoderMultiBuffer
{
    std::vector<DecoderBuffer> buffers;
    DecoderBuffer single;
    size_t totalSize{0};
};

class Decoder
{
public:
    Decoder() = default;
    virtual ~Decoder() {};
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual DecoderMultiBuffer decodeFrame() = 0;
    virtual DecoderMultiBuffer flush() = 0;
};

struct DecoderBaseConfiguration
{
    uint32_t width;
    uint32_t height;
    uint16_t fps_num;
    uint16_t fps_den;
};

template <typename T>
class DecoderWithConfiguration
{
private:
    T config;

public:
    DecoderWithConfiguration() = default;
    virtual void open(const T &config)
    {
        this->config = config;
    }
    virtual const T &getConfiguration() const
    {
        return config;
    }
    virtual void close() = 0;
    virtual ~DecoderWithConfiguration() {}
};