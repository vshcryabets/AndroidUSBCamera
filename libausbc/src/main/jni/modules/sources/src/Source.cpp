#include "Source.h"

namespace auvc {
    std::future<void> completed() {
        std::promise<void> promise;
        promise.set_value();
        return promise.get_future();
    }
}

const Source::OpenConfiguration Source::getOpenConfiguration() const
{
    return this->sourceConfig;
}

const Source::ProducingConfiguration Source::getProducingConfiguration() const
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