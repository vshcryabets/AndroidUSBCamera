#pragma once
#include "DataTypes.h"

namespace auvc {

class Consumer {
public:
    virtual ~Consumer() = default;
    virtual void consume(const Frame& frame) = 0;
    virtual void stopConsuming() = 0;
};

} // namespace auvc