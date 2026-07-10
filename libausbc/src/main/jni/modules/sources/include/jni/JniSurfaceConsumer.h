#pragma once

#include "jni.h"
#include <atomic>
#include <mutex>

#include "Consumer.h"

namespace auvc::jni {

class JniSurfaceConsumer: public auvc::OpenCloseConsumer {
private:
    std::atomic<bool> consuming {false};
    std::mutex jniConsumerMutex;
    jobject jniConsumer {nullptr};
    JavaVM* g_jvm;
    ANativeWindow *nativeWindow;
public:
    JniSurfaceConsumer(JavaVM* g_jvm);
    virtual ~JniSurfaceConsumer() override;
    void consume(const auvc::Frame& frame) override;
    auvc::ConsumerError setOpenConfiguration(
        JNIEnv* env,
        jobject jniConsumer,
        jobject surface
    );
    auvc::ConsumerError startConsuming() override;
    auvc::ConsumerError stopConsuming() override;
};

}