#include "TestSource.h"
#include <iostream>

TestSource::TestSource(const uint8_t *customFont): customFont(customFont)
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

auvc::Frame TestSource::readFrame()
{
    frameCounter++;
    if (testData)
    {
        uint16_t chunkWidth = captureConfiguration.width / testRGBAColors.size();
        uint32_t pixelOffset = 0;
        for (size_t y = 0; y < captureConfiguration.height; y++)
        {
            for (size_t x = 0; x < captureConfiguration.width; x++){
                uint32_t color = testRGBAColors[x / chunkWidth];
                testData[pixelOffset + 0] = (color >> 24) & 0xFF; // R
                testData[pixelOffset + 1] = (color >> 16) & 0xFF; // G
                testData[pixelOffset + 2] = (color >> 8) & 0xFF; // B
                testData[pixelOffset + 3] = 0xFF; // A
                pixelOffset += 4;
            }
        }
        drawString(sourceName, 20, 20, 1);
        drawString("Frame: " + std::to_string(frameCounter), 20, 30, 1);

        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - captureStartTime).count();
        int hours = static_cast<int>(elapsed / (1000 * 60 * 60));
        int minutes = static_cast<int>((elapsed / (1000 * 60)) % 60);
        int seconds = static_cast<int>((elapsed / 1000) % 60);
        int millis = static_cast<int>(elapsed % 1000);
        char timeStr[32];
        snprintf(timeStr, sizeof(timeStr), "Time %03d:%02d:%02d.%03d", 
            hours, minutes, 
            seconds, millis);
        drawString(std::string(timeStr), 20, 40, 1);

        return auvc::Frame(
            captureConfiguration.width, 
            captureConfiguration.height, 
            auvc::FrameFormat::RGBA,
            testData,
            testDataSize,
            std::chrono::high_resolution_clock::now()
        );
    }
    throw SourceError(SourceError::SOURCE_ERROR_CAPTURE_NOT_STARTED, 
        "Capture not started or invalid configuration");
}

std::future<void> TestSource::startProducing(const Source::ProducingConfiguration &config)
{
    return std::async(std::launch::deferred, [this, config]() {
        Source::startProducing(config).get(); // Call base implementation
        // Validate configuration 
        if (config.width == 0 ||
            config.height == 0 ||
            (config.width % testRGBAColors.size() != 0))
        {
            throw SourceError(SourceError::SOURCE_ERROR_WRONG_CONFIG, 
                "Invalid capture configuration, width,height must be > 0 and width % " + 
                std::to_string(testRGBAColors.size()) + " == 0");
        }
        testDataSize = config.width * config.height * 4;
        testData = new uint8_t[testDataSize];
        captureStartTime = std::chrono::steady_clock::now();
        sourceName = "TestSource RGBA " + std::to_string(config.width) + "x" + std::to_string(config.height);
    });
}

std::future<void> TestSource::close()
{
    return stopProducing();
}

std::vector<auvc::FrameFormat> TestSource::getSupportedFrameFormats() const
{
    return {auvc::FrameFormat::RGBA};
}

std::future<void> TestSource::stopProducing()
{
    return std::async(std::launch::deferred, [this]() {
        frameCounter = 0;
        if (testData)
        {
            delete[] testData;
            testData = nullptr;
        }
        testDataSize = 0;
    });
}

bool TestSource::waitNextFrame()
{
    return true; // Always return true for test source
}

void TestSource::drawChar(char c, uint16_t x, uint16_t y, uint8_t upscale) 
{
    if (customFont && testData)
    {
        if (upscale < 1) {
            upscale = 1; // Ensure upscale is at least 1u   
        }
        uint16_t stride = captureConfiguration.width * 4; // 4 bytes per pixel (RGBA)
        uint8_t min = customFont[0];
        uint8_t max = customFont[1];
        if (c < min || c > max) {
            return;
        }
        uint32_t fgColor = 0xFFFFFFFF; // White color for the character;
        uint32_t bgColor = 0x404040FF;
        uint32_t color;
        uint16_t idx = c - min;
        const uint8_t* verticalSymbols = customFont + 2 + idx * 8;
        for (uint8_t sy = 0; sy < 8; sy++) {
            uint8_t mask = 1 << sy;
            uint32_t lineOffset = (y + sy) * stride + x * 4;
            for (uint8_t sx = 0; sx < 8; sx++) {
                if (verticalSymbols[sx] & mask) {
                    color = fgColor; // Foreground color
                } else {
                    color = bgColor; // Background color
                }
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

void TestSource::drawString(std::string str, uint16_t x, uint16_t y, uint8_t upscale)
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

std::map<uint16_t, std::vector<Source::Resolution>> TestSource::getSupportedResolutions() const
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