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
        startPromise->set_value();
        PushSource::startProducing(config).get();
        std::cout << "PullToPushSource worker thread started" << std::endl;
        while (!stopRequested.load()) {
            if (pullSource->waitNextFrame()) {
                if (stopRequested.load()) {
                    break;
                }
                std::cout << "PullToPushSource: reading frame..." << std::endl;
                auto frame = pullSource->readFrame();
                if (stopRequested.load()) {
                    break;
                }
                if (frame.has_value()) {
                    std::cout << "PullToPushSource: pushing frame..." << std::endl;
                    this->pushFrame(frame.value());
                }
            }
        }
        running.store(false);
        stopRequested.store(false);
        std::cout << "PullToPushSource worker thread stopped" << std::endl;
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
