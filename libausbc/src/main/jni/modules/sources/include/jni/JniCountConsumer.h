#pragma once

#include "jni.h"
#include <atomic>
#include <mutex>

#include "Consumer.h"

class JniCountConsumer: public auvc::OpenCloseConsumer {
private:
    std::atomic<int> frameCount {0};
    std::atomic<bool> consuming {false};
    std::mutex jniConsumerMutex;
    jobject jniConsumer {nullptr};
    JavaVM* g_jvm;
public:
    JniCountConsumer(JavaVM* g_jvm);
    virtual ~JniCountConsumer() override;
    void consume(const auvc::Frame& frame) override;
    void setOpenConfiguration(
        jobject jniConsumer
    );
    auvc::ConsumerError startConsuming() override;
    auvc::ConsumerError stopConsuming() override;
    jobject getJniConsumer() const { return jniConsumer; }
};