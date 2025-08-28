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

void PullToPushSource::close()
{
    stopProducing();
    pullSource = nullptr;
}

std::map<uint16_t, std::vector<Source::Resolution>> 
PullToPushSource::getSupportedResolutions() const
{
    return {};
}

std::vector<auvc::FrameFormat> 
PullToPushSource::getSupportedFrameFormats() const
{
    return {};
}

void PullToPushSource::startProducing(const Source::ProducingConfiguration &config) 
{
    // start worker thread that pulls frames from pullSource and pushes them via pushFrame
    if (!pullSource) {
        throw SourceError(SourceError::SOURCE_ERROR_WRONG_CONFIG, "Pull source not set");
    }
    if (!pullSource->isReadyForProducing()) {
        throw SourceError(SourceError::SOURCE_ERROR_CAPTURE_NOT_STARTED, "Pull source not started");
    }
    running = true;
    workerThread = std::thread([this]() {
        while (running) {
            if (pullSource->waitNextFrame()) {
                if (!running) {
                    break;
                }
                auto frame = pullSource->readFrame();
                if (!running) {
                    break;
                }
                this->pushFrame(frame);
            }
        }
    });
}

void PullToPushSource::stopProducing()
{
    // stop worker thread
    running = false;
    if (workerThread.joinable()) {
        workerThread.join();
    }
}
