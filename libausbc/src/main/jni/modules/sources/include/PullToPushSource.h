#pragma once
#include "PushSource.h"
#include <memory>
#include <thread>
#include <atomic>

class PullToPushSource: public auvc::PushSource {
public:
    struct OpenConfiguration: public PushSource::OpenConfiguration {
        std::shared_ptr<PullSource> pullSource;
    };
protected:
    std::shared_ptr<PullSource> pullSource {nullptr};
    std::thread workerThread;
    std::atomic<bool> running {false};
public:
    PullToPushSource();
    virtual ~PullToPushSource();
    virtual void open(const OpenConfiguration &config);
    void startProducing(const ProducingConfiguration &config) override;
    void stopProducing() override;
    void close() override;
    std::map<uint16_t, std::vector<Resolution>> getSupportedResolutions() const override;
    std::vector<auvc::FrameFormat> getSupportedFrameFormats() const override;
};