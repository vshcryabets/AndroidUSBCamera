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