#include <iostream>
#include <memory>

#include "jni.h"

#include "jni/JniCountConsumer.h"
#include "jni/JniSources.h"
#include "jni/JniSourcesRepo.h"
#include "jni/JniThreadAttacher.h"
#include "jni/JniObjectError.h"
#include "Consumer.h"

using namespace auvc::jni;

extern "C"
JNIEXPORT jint JNICALL
Java_com_vsh_source_CountConsumer_nativeCreate(
        JNIEnv *env,
        jobject thiz) {
    JavaVM* g_jvm;
    env->GetJavaVM(&g_jvm);
    return JniSourcesRepo::getInstance()->addConsumer(std::make_shared<JniCountConsumer>(g_jvm));
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_vsh_source_CountConsumer_nativeRelease(JNIEnv *env, jobject thiz, jint sourceId) {
    auto source = JniSourcesRepo::getInstance()->getConsumer(sourceId);
    JniSourcesRepo::getInstance()->removeConsumer(sourceId);
    if (source) {
        auto jniConsumer = std::dynamic_pointer_cast<JniCountConsumer>(source);
        return fromConsumerError(env, jniConsumer->stopConsuming());
    } else {
        return fromConsumerError(env, auvc::ConsumerError::NOT_FOUND);
    }
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_vsh_source_CountConsumer_nativeStopConsuming(JNIEnv *env, jobject thiz, jint sourceId) {
    auto source = std::dynamic_pointer_cast<JniCountConsumer>(
            JniSourcesRepo::getInstance()->getConsumer(sourceId));
    if (source) {
        return fromConsumerError(env, source->stopConsuming());
    } else {
        return fromConsumerError(env, auvc::ConsumerError::NOT_FOUND);
    }
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_vsh_source_CountConsumer_nativeStartConsuming(JNIEnv *env, jobject thiz, jint sourceId) {
    auto source = std::dynamic_pointer_cast<JniCountConsumer>(
            JniSourcesRepo::getInstance()->getConsumer(sourceId));
    if (source) {
        source->stopConsuming();
        source->setOpenConfiguration(env->NewGlobalRef(thiz));
        return fromConsumerError(env, source->startConsuming());
    } else {
        return fromConsumerError(env, auvc::ConsumerError::NOT_FOUND);
    }
}

namespace auvc::jni {

void JniCountConsumer::consume(const auvc::Frame &frame) {
    thread_local JniThreadAttacher attacher(g_jvm);
    JNIEnv* env = attacher.env;
    if (env == nullptr) return;

    jobject consumerObj = nullptr;
    {
        std::lock_guard<std::mutex> lock(jniConsumerMutex);
        if (consuming && jniConsumer != nullptr) {
            frameCount++;
            consumerObj = env->NewLocalRef(jniConsumer);
        }
    }

    if (consumerObj != nullptr) {
        jclass clazz = env->GetObjectClass(consumerObj);
        jmethodID methodId = env->GetMethodID(clazz, "consume", "(Lcom/vsh/source/Frame;)V");
        if (methodId != nullptr) {
            env->CallVoidMethod(consumerObj, methodId, nullptr);
        }
        env->DeleteLocalRef(clazz);
        env->DeleteLocalRef(consumerObj);
    }
}

JniCountConsumer::JniCountConsumer(JavaVM* g_jvm): 
    jniConsumer(nullptr), g_jvm(g_jvm) {

}

JniCountConsumer::~JniCountConsumer() {
    stopConsuming();
}

auvc::ConsumerError JniCountConsumer::startConsuming() {
    std::lock_guard<std::mutex> lock(jniConsumerMutex);
    if (jniConsumer == nullptr) {
        return auvc::ConsumerError(auvc::ConsumerErrorCode::WRONG_CONFIGURATION, "JniCountConsumer: jniConsumer is null");
    }
    consuming = true;
    return auvc::ConsumerError::SUCCESS;
}

auvc::ConsumerError JniCountConsumer::stopConsuming() {
    std::lock_guard<std::mutex> lock(jniConsumerMutex);
    consuming = false;
    if (jniConsumer != nullptr) {
        thread_local JniThreadAttacher attacher(g_jvm);
        JNIEnv* env = attacher.env;
        env->DeleteGlobalRef(jniConsumer);
        jniConsumer = nullptr;
    }
    return auvc::ConsumerError::SUCCESS;
}

void JniCountConsumer::setOpenConfiguration(
    jobject jniConsumer
) {
    std::lock_guard<std::mutex> lock(jniConsumerMutex);
    this->jniConsumer = jniConsumer;
}

}