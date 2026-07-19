#pragma once
#include "Consumer.h"
#include "Source.h"
#include <memory>
#include <functional>

namespace auvc {
    class PushSource : public Source {
    protected:
        std::shared_ptr<auvc::Consumer> consumer;
    public:
        PushSource() : Source() {}
        virtual ~PushSource() = default;
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
        virtual void attachConsumer(std::shared_ptr<auvc::Consumer> consumer) {
            this->consumer = consumer;
        }
        [[nodiscard]] std::shared_ptr<auvc::Consumer> getAttachedConsumer() const {
            return consumer;
        }
        [[nodiscard]] virtual bool isReadyForProducing() const override {
            return (consumer != nullptr);
        }
   };
}