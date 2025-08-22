#include "PullToPushSource.h"
#include <iostream>

PullToPushSource::PullToPushSource()
{

}

PullToPushSource::~PullToPushSource()
{

}

void PullToPushSource::open(const OpenConfiguration &config) {
    PushSource::open(config);
    this->pullSource = config.pullSource;
}

void PullToPushSource::close()
{
    stopCapturing();
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

void PullToPushSource::startCapturing(const Source::CaptureConfiguration &config) 
{
    // start worker thread that pulls frames from pullSource and pushes them via pushFrame
    if (!pullSource) {
        throw SourceError(SourceError::SOURCE_ERROR_WRONG_CONFIG, "Pull source not set");
    }
    if (!pullSource->isReadyForCapture()) {
        throw SourceError(SourceError::SOURCE_ERROR_CAPTURE_NOT_STARTED, "Pull source not started");
    }
    workerThread = std::thread([this]() {
        running = true;
        std::cout << "PullToPushSource: Worker thread started" << std::endl;
        while (running) {
            if (pullSource->waitNextFrame()) {
                auto frame = pullSource->readFrame();
                this->pushFrame(frame);
            }
        }
        std::cout << "PullToPushSource: Worker thread ended" << std::endl;
    });
}

void PullToPushSource::stopCapturing()
{
    // stop worker thread
    running = false;
    if (workerThread.joinable()) {
        workerThread.join();
    }
}
