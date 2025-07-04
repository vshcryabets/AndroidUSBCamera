#pragma once

#include "Source.h"

class TestSource: public PullSource {
    private:
        uint8_t* testData {nullptr};
        size_t testDataSize {0};
        std::vector<uint32_t> testRGBAColors;
    public:
        TestSource();
        virtual ~TestSource();
        virtual Frame readFrame() override;
        void startCapturing(const CaptureConfiguration &config) override;
        void close() override;
        std::map<uint16_t, std::vector<Resolution>> getSupportedResolutions() override;
        std::vector<Source::FrameFormat> getSupportedFrameFormats() override;
        void stopCapturing() override;
        bool waitNextFrame() override;
};