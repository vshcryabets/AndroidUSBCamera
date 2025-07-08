#include "FrameDataInjectUseCase.h"
#include <iostream>

FrameDataInjectUseCase::FrameDataInjectUseCase(uint16_t width, uint16_t height)
    : quadWidth(width), quadHeight(height) {}

uint8_t FrameDataInjectUseCase::get4bit(const char *buffer, uint8_t bufferSize, uint16_t bitPos) const
{
    uint8_t byteIndex = bitPos / 8;
    if (byteIndex >= bufferSize)
    {
        return 0; // Out of bounds, return 0
    }
    bool highNibble = (bitPos % 8) < 4;
    return buffer[byteIndex] >> (highNibble ? 4 : 0) & 0x0F;
}

FrameDataInjectUseCaseRGBXImpl::FrameDataInjectUseCaseRGBXImpl(
    uint16_t width,
    uint16_t height) : FrameDataInjectUseCase(width, height)
{
}

Source::Frame FrameDataInjectUseCaseRGBXImpl::injectData(
    const Source::Frame &frame,
    const char *data,
    uint8_t dataSize)
{
    if (frame.format != Source::FrameFormat::RGBX)
    {
        throw SourceError(SourceError::SOURCE_ERROR_WRONG_CONFIG, "Frame format is not RGBX");
    }
    Source::Frame newFrame;
    newFrame.data = new uint8_t[frame.size];
    newFrame.size = frame.size;
    newFrame.format = Source::FrameFormat::RGBX;
    newFrame.width = frame.width;
    newFrame.timestamp = frame.timestamp;

    uint8_t currentQuad = 0;
    uint8_t bitPos = 0;
    uint16_t bitsCount = dataSize * 8;
    bool writeHighPart = true;
    uint32_t rgb = ((dataSize >> 4) << 20) |
                   ((dataSize & 0x0F) << 12) |
                   (get4bit(data, dataSize, bitPos) << 4);
    setMiddleRgb(newFrame, currentQuad * quadWidth, 0, rgb);
    bitPos += 4;
    currentQuad++;

    while (bitPos < bitsCount)
    {
        rgb = (get4bit(data, dataSize, bitPos) << 20) |
              (get4bit(data, dataSize, bitPos + 4) << 12) |
              (get4bit(data, dataSize, bitPos + 8) << 4);
        bitPos += 12;
        setMiddleRgb(newFrame, currentQuad * quadWidth, 0, rgb);
        currentQuad++;
    }

    rgb = getMiddleRgb(newFrame, 0, 0);
    printf("set: rgb = %06X\n", rgb);

    return newFrame;
}

uint32_t FrameDataInjectUseCaseRGBXImpl::getMiddleRgb(
    const Source::Frame &frame,
    uint16_t x,
    uint16_t y) const
{
    size_t offset = (y * frame.width + x) * 4;
    uint32_t racc = 0;
    uint32_t gacc = 0;
    uint32_t bacc = 0;
    for (size_t i = 0; i < quadHeight; ++i)
    {
        for (size_t j = 0; j < quadWidth; ++j)
        {
            size_t idx = (i * frame.width + j) * 4;
            racc += static_cast<uint32_t>(frame.data[offset + idx]);
            gacc += static_cast<uint32_t>(frame.data[offset + idx + 1]);
            bacc += static_cast<uint32_t>(frame.data[offset + idx + 2]);
        }
    }
    racc /= (quadWidth * quadHeight);
    gacc /= (quadWidth * quadHeight);
    bacc /= (quadWidth * quadHeight);
    return (racc << 16) | (gacc << 8) | bacc;
}
uint8_t FrameDataInjectUseCaseRGBXImpl::getDataSize(const Source::Frame &frame, uint16_t x, uint16_t y) const
{
    uint32_t rgb = getMiddleRgb(frame, x, y);
    printf("getDataSize: rgb = %06X\n", rgb);
    return ((rgb >> 16) & 0xFF) / 16 << 4 |
           ((rgb >> 8) & 0xFF) / 16;
}

void FrameDataInjectUseCaseRGBXImpl::readData(const Source::Frame &frame, uint16_t x, uint16_t y, char *outBuffer) const
{
}

void FrameDataInjectUseCaseRGBXImpl::setMiddleRgb(
    Source::Frame &frame,
    uint16_t x,
    uint16_t y,
    uint32_t rgb) const
{
    size_t offset = (y * frame.width + x) * 4;
    for (size_t i = 0; i < quadHeight; ++i)
    {
        for (size_t j = 0; j < quadWidth; ++j)
        {
            size_t idx = (i * frame.width + j) * 4;
            frame.data[offset + idx] = static_cast<uint8_t>((rgb >> 16) & 0xFF);    // Red
            frame.data[offset + idx + 1] = static_cast<uint8_t>((rgb >> 8) & 0xFF); // Green
            frame.data[offset + idx + 2] = static_cast<uint8_t>(rgb & 0xFF);        // Blue
        }
    }
}