#pragma once
#include "PushSource.h"
#include <memory>
#include <thread>
#include <atomic>

namespace auvc {

class PullToPushSource: public auvc::PushSource, public auvc::Consumer {
protected:
    std::thread workerThread;
    std::atomic<bool> running {false};
    std::atomic<bool> stopRequested {false};
    std::unique_ptr<std::promise<void>> startPromise {nullptr};
    std::shared_ptr<PullSource> pullSource;
public:
    PullToPushSource();
    virtual ~PullToPushSource();
    std::future<void> startProducing(const ProducingConfiguration &config) override;
    std::future<void> stopProducing() override;
    std::future<void> close() override;
    auvc::ExpectedResolutions getSupportedResolutions() const override;
    void consume(const auvc::Frame &frame) override {};
    ConsumerError attachTo(std::shared_ptr<Source> source) override;
};

} // namespace auvc