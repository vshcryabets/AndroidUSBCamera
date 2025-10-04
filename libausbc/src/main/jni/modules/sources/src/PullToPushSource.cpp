#include "PullToPushSource.h"
#include <iostream>

PullToPushSource::PullToPushSource()
{

}

PullToPushSource::~PullToPushSource()
{
    close();
}

void PullToPushSource::open(const OpenConfiguration &config) {
    PushSource::open(config);
    this->pullSource = config.pullSource;
}

std::future<void> PullToPushSource::close()
{
    return std::async(std::launch::async, [this]() {
        PushSource::close().get();
        stopProducing().get();
        pullSource = nullptr;
    });
}

auvc::ExpectedResolutions PullToPushSource::getSupportedResolutions() const
{
    return {};
}

std::vector<auvc::FrameFormat> 
PullToPushSource::getSupportedFrameFormats() const
{
    return {};
}

std::future<void> PullToPushSource::startProducing(const Source::ProducingConfiguration &config) 
{
    // start worker thread that pulls frames from pullSource and pushes them via pushFrame
    if (!pullSource) {
        throw auvc::SourceError(auvc::SourceErrorCode::SOURCE_ERROR_WRONG_CONFIG, "Pull source not set");
    }
    if (!pullSource->isReadyForProducing()) {
        throw auvc::SourceError(auvc::SourceErrorCode::SOURCE_ERROR_CAPTURE_NOT_STARTED, "Pull source not started");
    }

    startPromise = std::make_unique<std::promise<void>>();
    stopRequested.store(false);
    workerThread = std::thread([this]() {
        running.store(true);
        startPromise->set_value();
        while (!stopRequested.load()) {
            if (pullSource->waitNextFrame()) {
                if (stopRequested.load()) {
                    break;
                }
                auto frame = pullSource->readFrame();
                if (stopRequested.load()) {
                    break;
                }
                if (frame.has_value()) {
                    this->pushFrame(frame.value());
                }
            }
        }
        running.store(false);
        stopRequested.store(false);
    });
    return startPromise->get_future();
}

std::future<void> PullToPushSource::stopProducing()
{
    // stop worker thread
    stopRequested.store(true);
    return std::async(std::launch::async, [this]() {
        if (workerThread.joinable()) {
            workerThread.join();
       }
    });
}
