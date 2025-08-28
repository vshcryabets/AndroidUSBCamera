#pragma once
#include "Consumer.h"
#include "Source.h"
#include <memory>
#include <functional>

namespace auvc {
    class PushSource : public Source {
    public:
        using FrameCallback = std::function<void(const auvc::Frame &frame)>;
        struct OpenConfiguration: public Source::OpenConfiguration {
            std::shared_ptr<auvc::Consumer> consumer {nullptr};
            FrameCallback frameCallback {nullptr};
        };
    protected:
        std::shared_ptr<auvc::Consumer> consumer;
        FrameCallback frameCallback;
    public:
        PushSource() : Source() {}
        virtual ~PushSource() = default;
        virtual void open(const OpenConfiguration &config) {
            Source::open(config);
            this->consumer = config.consumer;
            this->frameCallback = config.frameCallback;
        }
        void close() override {
            consumer = nullptr;
            frameCallback = nullptr;
        }
        virtual void pushFrame(const auvc::Frame &frame) {
            if (frameCallback) {
                frameCallback(frame);
            }
            if (consumer) {
                consumer->consume(frame);
            }
        }
        [[nodiscard]] bool isPullSource() const override {
            return false;
        }
        [[nodiscard]] bool isPushSource() const override {
            return true;
        }
    };
}