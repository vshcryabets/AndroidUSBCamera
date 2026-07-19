#include "PullToPushSource.h"

namespace auvc {

PullToPushSource::PullToPushSource()
{

}

PullToPushSource::~PullToPushSource()
{
    close();
}

std::future<void> PullToPushSource::close()
{
    return std::async(std::launch::async, [this]() {
        PushSource::close().get();
        stopProducing().get();
        source = nullptr;
    });
}

auvc::ExpectedResolutions PullToPushSource::getSupportedResolutions() const
{
    return {};
}

std::future<void> 
PullToPushSource::startProducing(const ProducingConfiguration &config) 
{
    // start worker thread that pulls frames from pullSource and pushes them via pushFrame
    if (!source) {
        throw auvc::SourceError(auvc::SourceErrorCode::SOURCE_ERROR_WRONG_CONFIG, "Pull source not set");
    }
    if (!source->isReadyForProducing()) {
        throw auvc::SourceError(
            auvc::SourceErrorCode::SOURCE_ERROR_CAPTURE_NOT_STARTED, 
            "PullToPushSource::startProducing: Pull (input) source not started"
        );
    }
    if (consumer == nullptr) {
        throw auvc::SourceError(auvc::SourceErrorCode::SOURCE_ERROR_WRONG_CONFIG, "No consumer");
    }

    startPromise = std::make_unique<std::promise<void>>();
    stopRequested.store(false);
    workerThread = std::thread([this, config]() {
        running.store(true);
        PushSource::startProducing(config).get();
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

}