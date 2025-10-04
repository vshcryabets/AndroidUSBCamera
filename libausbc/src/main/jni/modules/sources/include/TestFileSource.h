#pragma once
#include "TestSource.h"
#include <fstream>
#include <vector>
#include <map>
#include <Consumer.h>

class TestFileSource : public PullSource {
public:
    struct ConnectConfiguration: public Source::OpenConfiguration {
        std::string fileName;
    };
private:
    // Header structure: 4 bytes magic + 2 bytes width + 2 bytes height + 4 bytes fps = 12 bytes
    const static uint32_t MAGIC_SIZE = 4;
    const static uint32_t WIDTH_SIZE = 2;
    const static uint32_t HEIGHT_SIZE = 2;
    const static uint32_t FPS_SIZE = 4;
    const static uint32_t TOC_POSITION = MAGIC_SIZE + WIDTH_SIZE + HEIGHT_SIZE + FPS_SIZE;
    uint16_t width, height;
    float fps;
    uint32_t currentFrame = 0;
    uint32_t framesCount = 0;
    uint32_t fileTocPosition = 0;
    std::ifstream dataFile;
    std::vector<uint32_t> framesTocItems;
    std::vector<auvc::FrameFormat> supportedFormats;
    std::map<uint16_t, std::vector<auvc::Resolution>> supportedResolutions;
public:
    TestFileSource();
    virtual ~TestFileSource();
    auvc::Frame readFrame() override;
    std::future<void> startProducing(const ProducingConfiguration &config) override;
    std::future<void> close() override;
    auvc::ExpectedResolutions getSupportedResolutions() const override;
    std::vector<auvc::FrameFormat> getSupportedFrameFormats() const override;
    std::future<void> stopProducing() override;
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

class TestFileWriter: auvc::Consumer {
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

    void consume(const auvc::Frame& frame) override;
    void stopConsuming() override;
    uint32_t getFramesCount() const {
        return framesCount;
    }
};
