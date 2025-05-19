#include "TestSource.h"
#include <iostream>

TestSource::TestSource()
{
    testRGBAColors = {
        0x000000FF, // Black
        0xFF0000FF, // Red
        0x00FF00FF, // Green
        0x0000FFFF, // Blue
        0xFFFF00FF, // Yellow
        0x00FFFFFF, // Cyan
        0xFF00FFFF, // Magenta
        0xFFFFFFFF, // White
    };
}
TestSource::~TestSource()
{
    close();
}

Source::Frame TestSource::readFrame()
{
    frameCounter++;
    Frame frame;
    frame.data = nullptr;
    frame.size = 0;
    if (testData)
    {
        uint16_t chunkWidth = captureConfigutation.width / testRGBAColors.size();
        uint32_t pixelOffset = 0;
        for (size_t y = 0; y < captureConfigutation.height; y++)
        {
            for (size_t x = 0; x < captureConfigutation.width; x++){
                uint32_t color = testRGBAColors[x / chunkWidth];
                testData[pixelOffset + 0] = (color >> 24) & 0xFF; // R
                testData[pixelOffset + 1] = (color >> 16) & 0xFF; // G
                testData[pixelOffset + 2] = (color >> 8) & 0xFF; // B
                testData[pixelOffset + 3] = 0xFF; // A
                pixelOffset += 4;
            }
        }
        frame.data = testData;
        frame.size = testDataSize;
        frame.timestamp = std::chrono::high_resolution_clock::now();
        frame.format = FrameFormat::RGBA;
    }

    return frame;
}

void TestSource::startCapturing(const Source::CaptureConfiguration &config)
{
    Source::startCapturing(config);
    if (config.width == 0 ||
        config.height == 0 ||
        (config.width % testRGBAColors.size() != 0))
    {
        throw SourceError(SourceError::SOURCE_ERROR_WRONG_CONFIG, "Invalid capture configuration, width,height must be > 0 and width % " + std::to_string(testRGBAColors.size()) + " == 0");
    }
    testDataSize = config.width * config.height * 4;
    testData = new uint8_t[testDataSize];
}

void TestSource::close()
{
    stopCapturing();
}

std::vector<Source::FrameFormat> TestSource::getSupportedFrameFormats()
{
    return {Source::FrameFormat::RGBA};
}

void TestSource::stopCapturing()
{
    if (testData)
    {
        delete[] testData;
        testData = nullptr;
    }
}

bool TestSource::waitNextFrame()
{
    return true; // Always return true for test source
}