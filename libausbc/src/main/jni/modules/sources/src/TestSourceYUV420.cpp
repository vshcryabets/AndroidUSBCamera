#include "TestSourceYUV420.h"
#include <iostream>
#include <thread>

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

auvc::Frame TestSourceYUV420::readFrame()
{
    frameCounter++;
    nextFrameTime = std::chrono::steady_clock::now() + frameInterval;
    if (testData)
    {
        const uint16_t width = captureConfiguration.width;
        const uint16_t chunkWidth = width / testYUVColors.size();
        const uint16_t halfWidth = width / 2;
        const uint16_t halfHeight = captureConfiguration.height / 2;

        for (size_t y = 0; y < captureConfiguration.height; y++)
        {
            for (size_t x = 0; x < width; x++){
                const YUVColor color = testYUVColors[x / chunkWidth];
                testData[y * width + x] = color.y;
                testDataU[y/2 * halfWidth + x / 2] = color.u;
                testDataV[y/2 * halfWidth + x / 2] = color.v;
            }
        }

        for (int y = halfHeight; y < captureConfiguration.height; ++y)
        {
            for (int x = halfWidth; x < width; ++x)
            {
                testData[y * width + x] = (x + y + frameCounter * 5) % 256; // Y plane
            }
        }
        for (int y = halfHeight/2; y < halfHeight; ++y)
        {
            for (int x = halfWidth/2; x < halfWidth; ++x)
            {
                testDataU[y * halfWidth + x] = (x + y + frameCounter * 2) % 256; // U plane
                testDataV[y * halfWidth + x] = (x + y + frameCounter * 3) % 256; // V plane
            }
        }

        drawString(sourceName, 20, 20, 1);
        drawString("Frame: " + std::to_string(frameCounter), 20, 30, 1);

        const auto now = std::chrono::steady_clock::now();
        const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - captureStartTime).count();
        const int hours = static_cast<int>(elapsed / (1000 * 60 * 60));
        const int minutes = static_cast<int>((elapsed / (1000 * 60)) % 60);
        const int seconds = static_cast<int>((elapsed / 1000) % 60);
        const int millis = static_cast<int>(elapsed % 1000);
        char timeStr[32];
        snprintf(timeStr, sizeof(timeStr), "Time %03d:%02d:%02d.%03d", 
            hours, minutes, 
            seconds, millis);
        drawString(std::string(timeStr), 20, 40, 1);

        return auvc::Frame(
            captureConfiguration.width, 
            captureConfiguration.height, 
            auvc::FrameFormat::YUV420P,
            testData,
            testDataSize,
            std::chrono::high_resolution_clock::now()
        );
    }
    throw SourceError(SourceError::SOURCE_ERROR_CAPTURE_NOT_STARTED, 
        "Capture not started or invalid configuration");
}

void TestSourceYUV420::startProducing(const Source::ProducingConfiguration &config)
{
    Source::startProducing(config);
    if (config.width == 0 ||
        config.height == 0 ||
        (config.width % testYUVColors.size() != 0))
    {
        throw SourceError(SourceError::SOURCE_ERROR_WRONG_CONFIG, 
            "Invalid capture configuration, width,height must be > 0 and width % " +
             std::to_string(testYUVColors.size()) + " == 0");
    }
    size_t pixelsCount = config.width * config.height;
    testDataSize = pixelsCount + pixelsCount / 2; // YUV420 requires 1.5 bytes per pixel
    testData = new uint8_t[testDataSize];
    testDataU = testData + pixelsCount; // U plane starts after Y plane
    testDataV = testDataU + pixelsCount / 4;
    captureStartTime = std::chrono::steady_clock::now();
    frameInterval = std::chrono::milliseconds(static_cast<int>(1000.0f / config.fps));
    sourceName = "TestSource YUV420p " + std::to_string(config.width) + "x" + std::to_string(config.height);
}

void TestSourceYUV420::close()
{
    stopProducing();
}

std::vector<auvc::FrameFormat> TestSourceYUV420::getSupportedFrameFormats() const
{
    return {auvc::FrameFormat::YUV420P};
}

void TestSourceYUV420::stopProducing()
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
    if (nextFrameTime.time_since_epoch().count() == 0) {
        return true;
    }
    std::this_thread::sleep_until(nextFrameTime);
    return true;
}

void TestSourceYUV420::drawChar(char c, uint16_t x, uint16_t y, uint8_t upscale) 
{
    if (customFont && testData)
    {
        if (upscale < 1) {
            upscale = 1; // Ensure upscale is at least 1u   
        }
        uint8_t min = customFont[0];
        uint8_t max = customFont[1];
        if (c < min || c > max) {
            return;
        }
        YUVColor fgColor = {255, 128, 128}; // White color for the character;
        YUVColor bgColor = {0x40, 128, 128};
        YUVColor color;

        uint16_t idx = c - min;
        const uint16_t stride = captureConfiguration.width;
        const uint8_t* verticalSymbols = customFont + 2 + idx * 8;
        for (uint8_t sy = 0; sy < 8; sy++) {
            uint8_t mask = 1 << sy;
            for (uint8_t sx = 0; sx < 8; sx++) {
                if (verticalSymbols[sx] & mask) {
                    color = fgColor; // Foreground color
                } else {
                    color = bgColor; // Background color
                }
                testData[(y + sy) * stride + x + sx] = color.y;
                size_t uvOffset = ((y + sy) / 2) * (stride / 2) + ((x + sx) / 2);
                testDataU[uvOffset] = color.u;
                testDataV[uvOffset] = color.v;
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

std::map<uint16_t, std::vector<Source::Resolution>> TestSourceYUV420::getSupportedResolutions() const
{
    std::map<uint16_t, std::vector<Source::Resolution>> result;
    std::vector<Source::Resolution> resoltions {
        {1, 640, 480, {30.0f, 60.0f}},
        {1, 1280, 720, {30.0f, 60.0f}},
        {1, 1920, 1080, {30.0f, 60.0f}},
        {1, 3840, 2160, {30.0f}}
    };
    result[0] = std::move(resoltions);
    return result;
}