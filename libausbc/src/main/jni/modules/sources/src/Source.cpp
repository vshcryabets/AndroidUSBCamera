#include "Source.h"

const Source::OpenConfiguration Source::getOpenConfiguration() const
{
    return this->sourceConfig;
}

const Source::CaptureConfiguration Source::getCaptureConfiguration() const
{
    return this->captureConfiguration;
}

const char* SourceError::what() const noexcept {
    return message.c_str();
}

bool Source::isReadyForCapture() const {
    return captureConfiguration.width != 0 && 
    captureConfiguration.height != 0 && 
    captureConfiguration.fps != 0.0f;
}