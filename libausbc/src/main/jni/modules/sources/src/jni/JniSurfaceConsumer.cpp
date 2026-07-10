#include <iostream>
#include <memory>

#include "jni.h"
#include <android/native_window_jni.h>

#include "jni/JniSurfaceConsumer.h"
#include "jni/JniSources.h"
#include "jni/JniSourcesRepo.h"
#include "jni/JniThreadAttacher.h"
#include "Consumer.h"

extern "C"
JNIEXPORT jint JNICALL
Java_com_vsh_source_SurfaceConsumer_nativeCreate(
        JNIEnv *env,
        jobject thiz) {
    JavaVM* g_jvm;
    env->GetJavaVM(&g_jvm);
    return JniSourcesRepo::getInstance()->addConsumer(std::make_shared<JniSurfaceConsumer>(g_jvm));
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_vsh_source_SurfaceConsumer_nativeRelease(JNIEnv *env, jobject thiz, jint sourceId) {
    auto source = JniSourcesRepo::getInstance()->getConsumer(sourceId);
    if (source) {
        auto jniConsumer = std::dynamic_pointer_cast<JniSurfaceConsumer>(source);
        if (jniConsumer) {
            jniConsumer->stopConsuming();
        }
    }
    JniSourcesRepo::getInstance()->removeConsumer(sourceId);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_vsh_source_SurfaceConsumer_nativeStopConsuming(JNIEnv *env, jobject thiz, jint sourceId) {
    auto source = std::dynamic_pointer_cast<JniSurfaceConsumer>(JniSourcesRepo::getInstance()->getConsumer(sourceId));
    if (source) {
        source->stopConsuming();
    }
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_vsh_source_SurfaceConsumer_nativeStartConsuming(JNIEnv *env, jobject thiz, jint sourceId) {
    auto source = std::dynamic_pointer_cast<JniSurfaceConsumer>(JniSourcesRepo::getInstance()->getConsumer(sourceId));
    if (source) {
        source->stopConsuming();
        source->setOpenConfiguration(env->NewGlobalRef(thiz));
        source->startConsuming();
    }
}

namespace auvc::jni {

void JniSurfaceConsumer::consume(const auvc::Frame &frame) {
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

JniSurfaceConsumer::JniSurfaceConsumer(JavaVM* g_jvm): 
    jniConsumer(nullptr), g_jvm(g_jvm) {

}

JniSurfaceConsumer::~JniSurfaceConsumer() {
    stopConsuming();
}

auvc::ConsumerError JniSurfaceConsumer::startConsuming() {
    std::lock_guard<std::mutex> lock(jniConsumerMutex);
    if (jniConsumer == nullptr) {
        return auvc::ConsumerError(auvc::ConsumerErrorCode::WRONG_CONFIGURATION, "JniSurfaceConsumer: jniConsumer is null");
    }
    consuming = true;
    return auvc::ConsumerError::SUCCESS;
}

auvc::ConsumerError JniSurfaceConsumer::stopConsuming() {
    std::lock_guard<std::mutex> lock(jniConsumerMutex);
    consuming = false;
    if (jniConsumer != nullptr) {
        thread_local JniThreadAttacher attacher(g_jvm);
        JNIEnv* env = attacher.env;
        env->DeleteGlobalRef(jniConsumer);
        jniConsumer = nullptr;
        if (nativeWindow != nullptr) {
            ANativeWindow_release(nativeWindow);
            nativeWindow = nullptr;
        }
    }
    return auvc::ConsumerError::SUCCESS;
}

void JniSurfaceConsumer::setOpenConfiguration(
    JNIEnv* env,
    jobject jniConsumer,
    jobject surface
) {
    std::lock_guard<std::mutex> lock(jniConsumerMutex);
    this->jniConsumer = jniConsumer;
    nativeWindow = ANativeWindow_fromSurface(env, surface);
}

}
