#pragma once

#include "jni.h"
#include <atomic>

#include "Consumer.h"

class JniCountConsumer: public auvc::OpenCloseConsumer {
private:
    std::atomic<int> frameCount {0};
    std::atomic<bool> consuming {false};
    jobject jniConsumer {nullptr};
    JavaVM* g_jvm;
public:
    JniCountConsumer(JavaVM* g_jvm);
    virtual ~JniCountConsumer() override;
    void consume(const auvc::Frame& frame) override;
    void setOpenConfiguration(
        jobject jniConsumer
    );
    auvc::ConsumerError openConsumer() override;
    auvc::ConsumerError closeConsumer() override;
    jobject getJniConsumer() const { return jniConsumer; }
};