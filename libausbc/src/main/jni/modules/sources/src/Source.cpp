#include "Source.h"

const Source::ConnectConfiguration Source::getSourceConfiguration()
{
    return this->sourceConfig;
}

const Source::CaptureConfiguration Source::getCaptureConfiguration()
{
    return this->captureConfiguration;
}

const char* SourceError::what() const noexcept {
    return message.c_str();
}