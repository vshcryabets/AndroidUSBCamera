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

void FrameDataInjectUseCase::injectData(
    Source::Frame &frame,
    const char *data,
    uint8_t dataSize) const
{
    uint8_t currentQuad = 0;
    uint16_t bitPos = 0;
    uint16_t bitsCount = dataSize * 8;
    if (((bitsCount + 8) / 12 + 1) > frame.width / quadWidth)
    {
        throw std::overflow_error("Data size exceeds available space in the frame.");
    }
    bool writeHighPart = true;
    uint32_t rgb = ((dataSize >> 4) << 20) |
                   ((dataSize & 0x0F) << 12) |
                   (get4bit(data, dataSize, bitPos) << 4) |
                   DEFAULT_LOW_NIBBLE_MASK;
    setMiddleRgb(frame, currentQuad * quadWidth, 0, rgb);
    bitPos += 4;
    currentQuad++;

    while (bitPos < bitsCount)
    {
        rgb = (get4bit(data, dataSize, bitPos) << 20) |
              (get4bit(data, dataSize, bitPos + 4) << 12) |
              (get4bit(data, dataSize, bitPos + 8) << 4) |
              DEFAULT_LOW_NIBBLE_MASK;
        bitPos += 12;
        setMiddleRgb(frame, currentQuad * quadWidth, 0, rgb);
        currentQuad++;
    }
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
uint16_t FrameDataInjectUseCase::getDataSize(const Source::Frame &frame, 
    uint16_t x, uint16_t y) const
{
    uint32_t rgb = getMiddleRgb(frame, x, y);
    return ((rgb >> 16) & 0xF0) | ((rgb >> 12) & 0x0F);
}

void FrameDataInjectUseCase::readData(const Source::Frame &frame, 
    uint16_t x, uint16_t y, 
    char *outBuffer, uint8_t outBufferMaxSize) const
{
    uint32_t rgb = getMiddleRgb(frame, x, y);
    uint16_t size = ((rgb >> 16) & 0xF0) | ((rgb >> 12) & 0x0F);
    uint16_t maxBitsCount = outBufferMaxSize * 8;
    uint16_t currentBitPos = 0;
    uint16_t currentQuad = 1;
    put4bit(outBuffer, outBufferMaxSize, currentBitPos, (rgb & 0xFF) >> 4);
    currentBitPos += 4;
    while (currentBitPos < maxBitsCount && currentBitPos < size * 8)
    {
        rgb = getMiddleRgb(frame, x + currentQuad * quadWidth, y);
        put4bit(outBuffer, outBufferMaxSize, currentBitPos, (rgb >> 20) & 0x0F);
        currentBitPos += 4;
        put4bit(outBuffer, outBufferMaxSize, currentBitPos, (rgb >> 12) & 0x0F);
        currentBitPos += 4;
        put4bit(outBuffer, outBufferMaxSize, currentBitPos, (rgb >> 4) & 0x0F);
        currentBitPos += 4;
        currentQuad++;
    }
    put4bit(outBuffer, outBufferMaxSize, currentBitPos, 0);
    currentBitPos += 4;
    put4bit(outBuffer, outBufferMaxSize, currentBitPos, 0);
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

void FrameDataInjectUseCase::put4bit(char* buffer, uint8_t bufferSize, uint16_t bitPos, uint8_t value) const
{
    if (bitPos / 8 >= bufferSize)
        return; // Out of bounds, do nothing
    uint8_t byteIndex = bitPos / 8;
    bool highNibble = (bitPos % 8) < 4;
    if (highNibble)
        buffer[byteIndex] = (buffer[byteIndex] & 0x0F) | (value << 4);
    else
        buffer[byteIndex] = (buffer[byteIndex] & 0xF0) | (value & 0x0F);
}

FrameDataInjectUseCaseYUV420pImpl::FrameDataInjectUseCaseYUV420pImpl(
    uint16_t width,
    uint16_t height) : FrameDataInjectUseCase(width, height)
{
}

uint32_t FrameDataInjectUseCaseYUV420pImpl::getMiddleRgb(const Source::Frame &frame, uint16_t x, uint16_t y) const
{
    size_t offset = y * frame.width + x;
    size_t uPlaneOffset = frame.width * frame.height;
    size_t vPlaneOffset = uPlaneOffset + (frame.width * frame.height) / 4;
    uPlaneOffset += offset / 4;
    vPlaneOffset += offset / 4;
    float yacc = 0;
    float uacc = 0;
    float vacc = 0;
    for (size_t i = 0; i < quadHeight; ++i)
    {
        for (size_t j = 0; j < quadWidth; ++j)
        {
            size_t idx = i * frame.width + j;
            yacc += frame.data[offset + idx];
            uacc += frame.data[uPlaneOffset + idx/4];
            vacc += frame.data[vPlaneOffset + idx/4];
        }
    }
    float yValue = yacc / (quadWidth * quadHeight);
    float uValue = uacc / (quadWidth * quadHeight);
    float vValue = vacc / (quadWidth * quadHeight);

    float rValue = yValue + 1.4075 * (vValue - 128);
    float gValue = yValue - 0.3455 * (uValue - 128) - (0.7169 * (vValue - 128));
    float bValue = yValue + 1.7790 * (uValue - 128);

    // Clamp to 0-255
    uint8_t racc = static_cast<uint8_t>(std::min(std::max((int)rValue, 0), 255));
    uint8_t gacc = static_cast<uint8_t>(std::min(std::max((int)gValue, 0), 255));
    uint8_t bacc = static_cast<uint8_t>(std::min(std::max((int)bValue, 0), 255));
    return (racc << 16) | (gacc << 8) | bacc;
}
void FrameDataInjectUseCaseYUV420pImpl::setMiddleRgb(Source::Frame &frame, uint16_t x, uint16_t y, uint32_t rgb) const 
{
    uint8_t rValue = (rgb >> 16) & 0xFF;
    uint8_t gValue = (rgb >> 8) & 0xFF;
    uint8_t bValue = rgb & 0xFF;
    // YUV conversion logic to set the Y, U, and V values in the frame
    uint8_t yValue = static_cast<uint8_t>(0.299 * rValue + 0.587 * gValue + 0.114 * bValue);
    uint8_t uValue = static_cast<uint8_t>(-0.169 * rValue - 0.331 * gValue + 0.5 * bValue + 128);
    uint8_t vValue = static_cast<uint8_t>(0.5 * rValue - 0.419 * gValue - 0.081 * bValue + 128);

    size_t uPlaneOffset = frame.width * frame.height;
    size_t vPlaneOffset = uPlaneOffset + (frame.width * frame.height) / 4;

    for (size_t i = 0; i < quadHeight; ++i)
    {
        for (size_t j = 0; j < quadWidth; ++j)
        {
            size_t yIndex = (y + i) * frame.width + (x + j);
            frame.data[yIndex] = yValue;
            frame.data[uPlaneOffset + yIndex / 4] = uValue;
            frame.data[vPlaneOffset + yIndex / 4] = vValue;
        }
    }
}