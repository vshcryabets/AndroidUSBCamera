#include "PullToPushSource.h"

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

void PullToPushSource::stopCapturing()
{

}

void PullToPushSource::close()
{

}

std::map<uint16_t, std::vector<Source::Resolution>> 
PullToPushSource::getSupportedResolutions() const
{
    return {};
}

std::vector<Source::FrameFormat> 
PullToPushSource::getSupportedFrameFormats() const
{
    return {};
}