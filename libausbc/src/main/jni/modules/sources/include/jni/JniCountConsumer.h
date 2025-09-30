#pragma once

#include "jni.h"
#include "Consumer.h"

void JniCountConsumer_register(JNIEnv *env);

class JniCountConsumer: public auvc::Consumer {
private:
    
public:
    JniCountConsumer() = default;
    ~JniCountConsumer() override = default;
    void consume(const auvc::Frame& frame) override;
    void stopConsuming() override;
};