#pragma once

#include "Source.h"

class TestSourceYUV420: public PullSource {
    private:
        struct YUVColor {
            uint8_t y;
            uint8_t u;
            uint8_t v;
        };
        uint8_t* testData {nullptr};
        uint8_t* testDataU {nullptr};
        uint8_t* testDataV {nullptr};
        size_t testDataSize {0};
        std::vector<YUVColor> testYUVColors;
        const uint8_t *customFont;
        std::chrono::steady_clock::time_point captureStartTime;
        std::string sourceName;
        std::chrono::steady_clock::time_point nextFrameTime;
        std::chrono::milliseconds frameInterval {0};
    private:
        void drawString(std::string str, uint16_t x, uint16_t y, uint8_t upscale = 1);
        void drawChar(char c, uint16_t x, uint16_t y, uint8_t upscale = 1);
    public:
        TestSourceYUV420(const uint8_t *customFont);
        virtual ~TestSourceYUV420();
        virtual auvc::Frame readFrame() override;
        std::future<void> startProducing(const ProducingConfiguration &config) override;
        std::future<void> close() override;
        std::vector<auvc::FrameFormat> getSupportedFrameFormats() const override;
        std::map<uint16_t, std::vector<Resolution>> getSupportedResolutions() const override;
        std::future<void> stopProducing() override;
        bool waitNextFrame() override;
};