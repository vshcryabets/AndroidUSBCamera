#include "jni/JniSourceError.h"

namespace auvc::jni
{

    JniSourceErrorType fromConsumerError(const auvc::ConsumerError &error)
    {
        switch (error.getCode())
        {
        case auvc::ConsumerErrorCode::SUCCESS:
            return JniSourceErrorType::SUCCESS;
        case auvc::ConsumerErrorCode::WRONG_CONFIGURATION:
            return JniSourceErrorType::INVALID_ARGUMENT;
        default:
            return JniSourceErrorType::SOURCE_NOT_INITIALIZED;
        }
    }

}
