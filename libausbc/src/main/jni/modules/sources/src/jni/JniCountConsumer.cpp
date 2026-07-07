#include <iostream>
#include <memory>

#include "jni/JniCountConsumer.h"
#include "jni/JniSources.h"
#include "jni/JniSourcesRepo.h"
#include "Consumer.h"

extern "C"
JNIEXPORT jint JNICALL
Java_com_vsh_source_CountConsumer_nativeCreate(
        JNIEnv *env,
        jobject thiz) {
    return JniSourcesRepo::getInstance()->addConsumer(std::make_shared<JniCountConsumer>());
}

extern "C"
JNIEXPORT void JNICALL
Java_com_vsh_source_CountConsumer_nativeRelease(JNIEnv *env, jobject thiz, jint sourceId) {
    auto source = JniSourcesRepo::getInstance()->getConsumer(sourceId);
    JniSourcesRepo::getInstance()->removeConsumer(sourceId);
    if (source) {
        auto jniConsumer = std::dynamic_pointer_cast<JniCountConsumer>(source);
        auto jniConsumerGlobal = jniConsumer->getJniConsumer();
        env->DeleteGlobalRef(jniConsumerGlobal);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_vsh_source_CountConsumer_nativeStopConsuming(JNIEnv *env, jobject thiz, jint sourceId) {
    auto source = std::dynamic_pointer_cast<JniCountConsumer>(JniSourcesRepo::getInstance()->getConsumer(sourceId));
    if (source) {
        source->closeConsumer();
    }
}

void JniCountConsumer::consume(const auvc::Frame &frame) {
    std::printf("ASD JniCountConsumer::consume: start\n");
    if (consuming) {
        frameCount++;
        std::printf("ASD JniCountConsumer::consume: counter %d\n", frameCount.load());
    }
}

JniCountConsumer::JniCountConsumer(): 
    jniConsumer(nullptr) {

}

JniCountConsumer::~JniCountConsumer() {
    
}

auvc::ConsumerError JniCountConsumer::openConsumer() {
    consuming = true;
    return auvc::ConsumerError::SUCCESS;
}

auvc::ConsumerError JniCountConsumer::closeConsumer() {
    consuming = false;
    return auvc::ConsumerError::SUCCESS;
}