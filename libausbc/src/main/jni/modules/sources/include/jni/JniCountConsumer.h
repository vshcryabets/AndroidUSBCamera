#pragma once

#include "jni.h"
#include <atomic>

#include "Consumer.h"

void JniCountConsumer_register(JNIEnv *env);

class JniCountConsumer: public auvc::Consumer {
private:
    std::atomic<int> frameCount {0};
    std::atomic<bool> consuming {false};
public:
    JniCountConsumer() = default;
    ~JniCountConsumer() override = default;
    void consume(const auvc::Frame& frame) override;
    void stopConsuming() override;
};