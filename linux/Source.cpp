#include "Source.h"

const Source::ConnectConfiguration Source::getSourceConfgiuration()
{
    return this->sourceConfig;
}

const Source::CaptureConfigutation Source::getCaptureConffiguration()
{
    return this->captureConfigutation;
}

const char* SourceError::what() const noexcept {
    return message.c_str();
}