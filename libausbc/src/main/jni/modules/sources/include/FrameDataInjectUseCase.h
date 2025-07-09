#pragma once
#include <cstdint>
#include "Source.h"

class FrameDataInjectUseCase
{
protected:
    uint16_t quadWidth;
    uint16_t quadHeight;
public:
    FrameDataInjectUseCase(uint16_t width, uint16_t height);
    virtual ~FrameDataInjectUseCase() = default;
    virtual void injectData(
        Source::Frame &frame,
        const char *data,
        uint8_t dataSize) const;
    virtual uint32_t getMiddleRgb(const Source::Frame &frame, 
        uint16_t x, uint16_t y) const = 0;
    virtual void setMiddleRgb(Source::Frame &frame, 
        uint16_t x, uint16_t y, uint32_t rgb) const = 0;
    virtual uint16_t getDataSize(const Source::Frame &frame, 
        uint16_t x, uint16_t y) const;
    virtual void readData(const Source::Frame &frame, 
        uint16_t x, uint16_t y, 
        char* outBuffer, uint8_t bufferMaxSize) const;
    uint8_t get4bit(const char* buffer, uint8_t bufferSize, uint16_t bitPos) const;
    void put4bit(char* buffer, uint8_t bufferSize, uint16_t bitPos, uint8_t value) const;
};

class FrameDataInjectUseCaseRGBXImpl : public FrameDataInjectUseCase
{
public:
    FrameDataInjectUseCaseRGBXImpl(uint16_t width, uint16_t height);
    virtual ~FrameDataInjectUseCaseRGBXImpl() = default;
    uint32_t getMiddleRgb(const Source::Frame &frame, uint16_t x, uint16_t y) const override;
    void setMiddleRgb(Source::Frame &frame, uint16_t x, uint16_t y, uint32_t rgb) const override;
};

class FrameDataInjectUseCaseYUV420pImpl : public FrameDataInjectUseCase
{
};