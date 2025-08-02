#pragma once
#include "TestSource.h"

class TestFileSource : public PullSource {
public:
    TestFileSource() = default;
    virtual ~TestFileSource() = default;
    virtual Frame readFrame() override;
    void startCapturing(const CaptureConfiguration &config) override;
    void close() override;
    std::map<uint16_t, std::vector<Resolution>> getSupportedResolutions() override;
    std::vector<Source::FrameFormat> getSupportedFrameFormats() override;
    void stopCapturing() override;
    bool waitNextFrame() override;
};

class TestFileWritter {
private:
    const static uint32_t TOC_POSITION = 12;
    uint32_t framesCount = 0;
    uint32_t filePosition = 0;
    std::ofstream *dataFile {nullptr};
    std::vector<uint32_t> framesTocItems;
public:
    TestFileWritter(const std::string &fileName,
                   uint16_t width, 
                   uint16_t height,
                   std::string mimeType,
                   float fps);
    ~TestFileWritter();
    
    void write(const uint8_t *data, uint32_t size);
    void finalize();
};
