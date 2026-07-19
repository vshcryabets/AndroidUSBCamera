#include "Source.h"

namespace auvc {

const SourceError SourceError::NOT_FOUND = SourceError(SourceErrorCode::OBJECT_NOT_FOUND, "Source object not found");
const SourceError SourceError::SUCCESS = SourceError(SourceErrorCode::SUCCESS, "Success");

std::future<void> completed() {
    std::promise<void> promise;
    promise.set_value();
    return promise.get_future();
}

const SourceConfiguration Source::getOpenConfiguration() const
{
    return this->sourceConfig;
}

const auvc::ProducingConfiguration Source::getProducingConfiguration() const
{
    return this->captureConfiguration;
}

const char* auvc::SourceError::what() const noexcept {
    return message.c_str();
}

bool Source::isReadyForProducing() const {
    return captureConfiguration.width != 0 && 
    captureConfiguration.height != 0 && 
    captureConfiguration.fps != 0.0f;
}

}