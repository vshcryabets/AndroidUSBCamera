#pragma once
#include "Source.h"
#include <memory>

class PullToPushSource: public PushSource {
public:
    struct OpenConfiguration: public PushSource::OpenConfiguration {
        std::shared_ptr<PullSource> pullSource;
    };
protected:
    std::shared_ptr<PullSource> pullSource {nullptr};
public:
    PullToPushSource() : PushSource() {}
    virtual ~PullToPushSource() = default;
    virtual void open(const OpenConfiguration &config) {
        PushSource::open(config);
        this->pullSource = config.pullSource;
    }
    void stopCapturing() override;
    void close() override;
    std::map<uint16_t, std::vector<Resolution>> getSupportedResolutions() const override;
    std::vector<FrameFormat> getSupportedFrameFormats() const override;
};