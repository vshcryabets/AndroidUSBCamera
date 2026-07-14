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

}
