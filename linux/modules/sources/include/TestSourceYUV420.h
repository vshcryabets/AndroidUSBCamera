#pragma once

#include "Source.h"

class TestSourceYUV420: public Source {
    private:
        uint8_t* testData {nullptr};
        size_t testDataSize {0};
        std::vector<uint32_t> testRGBAColors;
        const uint8_t *customFont;
    private:
        void drawString(std::string str, uint16_t x, uint16_t y, uint8_t upscale = 1);
        void drawChar(char c, uint16_t x, uint16_t y, uint8_t upscale = 1);
    public:
        TestSourceYUV420(const uint8_t *customFont);
        virtual ~TestSourceYUV420();
        virtual Frame readFrame() override;
        void startCapturing(const CaptureConfiguration &config) override;
        void close() override;
        std::vector<Source::FrameFormat> getSupportedFrameFormats() override;
        void stopCapturing() override;
        bool waitNextFrame() override;
};