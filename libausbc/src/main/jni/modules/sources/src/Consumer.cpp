#include "Consumer.h"

namespace auvc {

ConsumerError ConsumerError::SUCCESS = ConsumerError(ConsumerErrorCode::SUCCESS, "Success");
ConsumerError ConsumerError::NOT_FOUND = ConsumerError(ConsumerErrorCode::OBJECT_NOT_FOUND, "Consumer not found");

auvc::ConsumerError OpenCloseConsumer::startConsuming()
{
    return ConsumerError::SUCCESS;
}
auvc::ConsumerError OpenCloseConsumer::stopConsuming()
{
    return ConsumerError::SUCCESS;
}

const char* ConsumerError::what() const noexcept
{
    return message.c_str();
}

auvc::ConsumerError Consumer::attachTo(std::shared_ptr<Source> source)
{
    if (!source) {
        return ConsumerError(ConsumerErrorCode::WRONG_CONFIGURATION, "Source is null");
    }
    this->source = source;
    return ConsumerError::SUCCESS;
}

auvc::ConsumerError Consumer::startChain(std::queue<ProducingConfiguration> &openConfigurations)
{
    if (!source) {
        return ConsumerError(ConsumerErrorCode::OBJECT_NOT_FOUND, "Source not attached");
    }
    source->startProducing(source->getProducingConfiguration()).get();
    return ConsumerError::SUCCESS;

}

}
