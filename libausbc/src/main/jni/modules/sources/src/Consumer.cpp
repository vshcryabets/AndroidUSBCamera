#include "Consumer.h"

namespace auvc {

ConsumerError ConsumerError::SUCCESS = ConsumerError(ConsumerErrorCode::SUCCESS, "Success");

auvc::ConsumerError OpenCloseConsumer::openConsumer()
{
    return ConsumerError::SUCCESS;
}
auvc::ConsumerError OpenCloseConsumer::closeConsumer()
{
    return ConsumerError::SUCCESS;
}

const char* ConsumerError::what() const noexcept
{
    return message.c_str();
}

}
