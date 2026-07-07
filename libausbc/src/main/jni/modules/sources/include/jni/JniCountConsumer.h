#pragma once

#include "jni.h"
#include <atomic>

#include "Consumer.h"

class JniCountConsumer: public auvc::OpenCloseConsumer {
private:
    std::atomic<int> frameCount {0};
    std::atomic<bool> consuming {false};
    jobject jniConsumer {nullptr};
public:
    JniCountConsumer();
    virtual ~JniCountConsumer() override;
    void consume(const auvc::Frame& frame) override;
    auvc::ConsumerError openConsumer() override;
    auvc::ConsumerError closeConsumer() override;
    jobject getJniConsumer() const { return jniConsumer; }
};