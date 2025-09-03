#pragma once
#include <cstdint>
#include <vector>
#include <exception>
#include <string>
#include "Consumer.h"
#include "PushSource.h"

class DecoderException : public std::exception
{
public:
    enum Type
    {
        Unknown = 0,
        NotInitialized,
        FailedToDecodeFrame,
        DecoderNotFound,
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

class Decoder: public auvc::Consumer, public auvc::PushSource
{
public:
    Decoder() = default;
    virtual ~Decoder() {};
};

struct DecoderBaseConfiguration: auvc::PushSource::OpenConfiguration
{
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