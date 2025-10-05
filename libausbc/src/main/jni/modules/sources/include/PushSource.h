#pragma once
#include "Consumer.h"
#include "Source.h"
#include <memory>
#include <functional>

namespace auvc {
    class PushSource : public Source {
    public:
        struct OpenConfiguration: public Source::OpenConfiguration {
            std::shared_ptr<auvc::Consumer> consumer {nullptr};
        };
    protected:
        std::shared_ptr<auvc::Consumer> consumer;
    public:
        PushSource() : Source() {}
        virtual ~PushSource() = default;
        virtual void open(const OpenConfiguration &config) {
            Source::open(config);
            this->consumer = config.consumer;
        }
        std::future<void> close() override {
            return std::async(std::launch::async, [this]() {
                consumer = nullptr;
            });
        }
        virtual void pushFrame(const auvc::Frame &frame) {
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