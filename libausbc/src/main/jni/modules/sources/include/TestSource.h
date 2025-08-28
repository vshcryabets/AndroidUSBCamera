#pragma once

#include "Source.h"
#include <chrono>

class TestSource: public PullSource {
    private:
        uint8_t* testData {nullptr};
        size_t testDataSize {0};
        std::vector<uint32_t> testRGBAColors;
        const uint8_t *customFont;
        std::chrono::steady_clock::time_point captureStartTime;
        std::string sourceName;
    private:
        void drawString(std::string str, uint16_t x, uint16_t y, uint8_t upscale = 1);
        void drawChar(char c, uint16_t x, uint16_t y, uint8_t upscale = 1);
    public:
        TestSource(const uint8_t *customFont);
        virtual ~TestSource();
        virtual auvc::Frame readFrame() override;
        void startProducing(const ProducingConfiguration &config) override;
        void close() override;
        std::map<uint16_t, std::vector<Resolution>> getSupportedResolutions() const override;
        std::vector<auvc::FrameFormat> getSupportedFrameFormats() const override;
        void stopProducing() override;
        bool waitNextFrame() override;
};