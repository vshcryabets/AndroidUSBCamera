#pragma once
#include "TestSource.h"
#include <fstream>

class TestFileSource : public PullSource {
public:
    struct ConnectConfiguration: public Source::ConnectConfiguration {
        std::string fileName;
    };
private:
    const static uint32_t TOC_POSITION = 12;
    uint32_t currentFrame = 0;
    uint32_t framesCount = 0;
    uint32_t fileTocPosition = 0;
    std::ifstream dataFile;
    std::vector<uint32_t> framesTocItems;
    std::vector<Source::FrameFormat> supportedFormats;
    std::map<uint16_t, std::vector<Resolution>> supportedResolutions;
public:
    TestFileSource();
    virtual ~TestFileSource();
    Frame readFrame() override;
    void startCapturing(const CaptureConfiguration &config) override;
    void close() override;
    std::map<uint16_t, std::vector<Resolution>> getSupportedResolutions() override;
    std::vector<Source::FrameFormat> getSupportedFrameFormats() override;
    void stopCapturing() override;
    bool waitNextFrame() override;
    virtual void open(const ConnectConfiguration &config);
    uint32_t getFramesCount() const {
        return framesCount;
    }
    uint32_t setCurrentFrame(uint32_t frameIndex);
    uint32_t getCurrentFrame() const {
        return currentFrame;
    }
};

class TestFileWriter {
private:
    const static uint32_t TOC_POSITION = 12;
    uint32_t framesCount = 0;
    uint32_t filePosition = 0;
    std::ofstream dataFile;
    std::vector<uint32_t> framesTocItems;
public:
    TestFileWriter(const std::string &fileName,
                   uint16_t width, 
                   uint16_t height,
                   std::string mimeType,
                   float fps);
    ~TestFileWriter();
    
    void write(const uint8_t *data, uint32_t size);
    void finalize();
};
