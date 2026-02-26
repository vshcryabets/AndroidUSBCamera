#pragma once
#include "Consumer.h"
#include "PushSource.h"

namespace auvc {

    /**
     * Base class for frame transform stages in a pipeline.
     * Receives frames via Consumer::consume(), applies a transformation,
     * then forwards the result downstream via PushSource::pushFrame().
     *
     * Subclasses must implement:
     *   - consume(const Frame&)   – perform the transformation and call pushFrame()
     *   - stopConsuming()         – release any transform-specific resources
     */
    class Transform : public Consumer, public PushSource {
    public:
        Transform() : PushSource() {}
        virtual ~Transform() = default;

        // Receives an incoming frame. Subclass should process it and call pushFrame().
        void consume(const Frame& frame) override = 0;

        // Signals that no more frames will arrive; subclass should flush / clean up.
        void stopConsuming() override = 0;
    };

} // namespace auvc
