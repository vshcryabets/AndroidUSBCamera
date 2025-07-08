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
    virtual Source::Frame injectData(
        const Source::Frame &frame,
        const char *data,
        uint8_t dataSize) = 0;
    virtual uint32_t getMiddleRgb(const Source::Frame &frame, uint16_t x, uint16_t y) const = 0;
    virtual void setMiddleRgb(Source::Frame &frame, uint16_t x, uint16_t y, uint32_t rgb) const = 0;
    virtual uint8_t getDataSize(const Source::Frame &frame, uint16_t x, uint16_t y) const = 0;
    virtual void readData(const Source::Frame &frame, uint16_t x, uint16_t y, char* outBuffer) const = 0;
    uint8_t get4bit(const char* buffer, uint8_t bufferSize, uint16_t bitPos) const;
};

class FrameDataInjectUseCaseRGBXImpl : public FrameDataInjectUseCase
{
public:
    FrameDataInjectUseCaseRGBXImpl(uint16_t width, uint16_t height);
    virtual ~FrameDataInjectUseCaseRGBXImpl() = default;
    Source::Frame injectData(
        const Source::Frame &frame,
        const char *data,
        uint8_t dataSize) override;
    uint32_t getMiddleRgb(const Source::Frame &frame, uint16_t x, uint16_t y) const override;
    void setMiddleRgb(Source::Frame &frame, uint16_t x, uint16_t y, uint32_t rgb) const override;
    uint8_t getDataSize(const Source::Frame &frame, uint16_t x, uint16_t y) const override;
    void readData(const Source::Frame &frame, uint16_t x, uint16_t y, char* outBuffer) const override;
};

class FrameDataInjectUseCaseYUV420pImpl : public FrameDataInjectUseCase
{
};