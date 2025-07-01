#include "TestSourceYUV420.h"
#include <iostream>

TestSourceYUV420::TestSourceYUV420(const uint8_t *customFont): customFont(customFont)
{
    testYUVColors = {
        {0, 0x80, 0x80}, // Black
        {0x4C, 84, 255}, // Red
        {149, 43, 21}, // Green
        {29, 255, 107}, // Blue
        {225, 0, 148}, // Yellow
        {178, 171, 0}, // Cyan
        {105, 212, 234}, // Magenta
        {255, 128, 128}, // White
    };
}
TestSourceYUV420::~TestSourceYUV420()
{
    close();
}

Source::Frame TestSourceYUV420::readFrame()
{
    frameCounter++;
    Frame frame;
    frame.data = nullptr;
    frame.size = 0;
    if (testData)
    {
        uint16_t chunkWidth = captureConfigutation.width / testYUVColors.size();
        for (size_t y = 0; y < captureConfigutation.height; y++)
        {
            for (size_t x = 0; x < captureConfigutation.width; x++){
                size_t baseOffset = y * captureConfigutation.width + x;
                YUVColor color = testYUVColors[x / chunkWidth];
                testData[baseOffset] = color.y;
                testDataU[baseOffset / 4] = color.u;
                testDataV[baseOffset / 4] = color.v;
            }
        }
        drawString("Test Source", 20, 20, 1);
        drawString("Frame: " + std::to_string(frameCounter), 20, 30, 1);
        frame.data = testData;
        frame.size = testDataSize;
        frame.timestamp = std::chrono::high_resolution_clock::now();
        frame.format = FrameFormat::YUV420P;
    }
    return frame;
}

void TestSourceYUV420::startCapturing(const Source::CaptureConfiguration &config)
{
    Source::startCapturing(config);
    if (config.width == 0 ||
        config.height == 0 ||
        (config.width % testYUVColors.size() != 0))
    {
        throw SourceError(SourceError::SOURCE_ERROR_WRONG_CONFIG, "Invalid capture configuration, width,height must be > 0 and width % " + std::to_string(testYUVColors.size()) + " == 0");
    }
    size_t pixelsCount = config.width * config.height;
    testDataSize = pixelsCount + pixelsCount / 2; // YUV420 requires 1.5 bytes per pixel
    testData = new uint8_t[testDataSize];
    testDataU = testData + pixelsCount; // U plane starts after Y plane
    testDataV = testDataU + pixelsCount / 4;
}

void TestSourceYUV420::close()
{
    stopCapturing();
}

std::vector<Source::FrameFormat> TestSourceYUV420::getSupportedFrameFormats()
{
    return {Source::FrameFormat::RGBA};
}

void TestSourceYUV420::stopCapturing()
{
    if (testData)
    {
        delete[] testData;
        testData = nullptr;
        testDataU = nullptr;
        testDataV = nullptr;
        testDataSize = 0;
    }
}

bool TestSourceYUV420::waitNextFrame()
{
    return true; // Always return true for test source
}

void TestSourceYUV420::drawChar(char c, uint16_t x, uint16_t y, uint8_t upscale) 
{
    if (customFont && testData)
    {
        if (upscale < 1) {
            upscale = 1; // Ensure upscale is at least 1u   
        }
        uint16_t stride = captureConfigutation.width * 4; // 4 bytes per pixel (RGBA)
        uint8_t min = customFont[0];
        uint8_t max = customFont[1];
        if (c < min || c > max) {
            return;
        }
        uint32_t color = 0xFFFFFFFF; // White color for the character;
        uint16_t idx = c - min;
        const uint8_t* verticalSymbols = customFont + 2 + idx * 8;
        for (uint8_t sy = 0; sy < 8; sy++) {
            uint8_t mask = 1 << sy;
            uint32_t lineOffset = (y + sy) * stride + x * 4;
            for (uint8_t sx = 0; sx < 8; sx++) {
                if (verticalSymbols[sx] & mask) {
                    uint32_t pixelOffset = lineOffset + (sx * 4);
                    if (pixelOffset + 3 < testDataSize) {
                        testData[pixelOffset + 0] = (color >> 24) & 0xFF; // R
                        testData[pixelOffset + 1] = (color >> 16) & 0xFF; // G
                        testData[pixelOffset + 2] = (color >> 8) & 0xFF; // B
                        testData[pixelOffset + 3] = 0xFF; // A
                    }
                }
            }
        }
    }
}

void TestSourceYUV420::drawString(std::string str, uint16_t x, uint16_t y, uint8_t upscale)
{
    if (customFont && testData)
    {
        if (upscale < 1) {
            upscale = 1; // Ensure upscale is at least 1u   
        }
        for (size_t i = 0; i < str.size(); i++)
        {
            drawChar(str[i], x + i * 8 * upscale, y, upscale);
        }
    }
}