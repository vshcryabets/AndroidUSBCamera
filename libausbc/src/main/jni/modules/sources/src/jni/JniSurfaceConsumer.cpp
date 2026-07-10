#include <iostream>
#include <memory>

#include "jni.h"

#include "jni/JniSurfaceConsumer.h"
#include "jni/JniSources.h"
#include "jni/JniSourcesRepo.h"
#include "jni/JniThreadAttacher.h"
#include "jni/JniSourceError.h"
#include "Consumer.h"

using namespace auvc::jni;

extern "C"
JNIEXPORT jint JNICALL
Java_com_vsh_source_SurfaceConsumer_nativeCreate(
        JNIEnv *env,
        jobject thiz) {
    JavaVM *g_jvm;
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
    } else {
        return JniSourceErrorType::SOURCE_NOT_FOUND;
    }
    JniSourcesRepo::getInstance()->removeConsumer(sourceId);
    return JniSourceErrorType::SUCCESS;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_vsh_source_SurfaceConsumer_nativeStopConsuming(JNIEnv *env, jobject thiz, jint sourceId) {
    auto source = std::dynamic_pointer_cast<JniSurfaceConsumer>(
            JniSourcesRepo::getInstance()->getConsumer(sourceId)
    );
    if (source) {
        return fromConsumerError(source->stopConsuming());
    }
    return JniSourceErrorType::SOURCE_NOT_FOUND;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_vsh_source_SurfaceConsumer_nativeStartConsuming(
        JNIEnv *env,
        jobject thiz,
        jint sourceId) {
    auto source = std::dynamic_pointer_cast<JniSurfaceConsumer>(
            JniSourcesRepo::getInstance()->getConsumer(sourceId));
    if (source) {
        return fromConsumerError(source->startConsuming());
    }
    return JniSourceErrorType::SOURCE_NOT_FOUND;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_vsh_source_SurfaceConsumer_nativeSetOpenConfiguration(
        JNIEnv *env,
        jobject thiz,
        jint sourceId,
        jobject surface) {
    auto source = std::dynamic_pointer_cast<JniSurfaceConsumer>(
            JniSourcesRepo::getInstance()->getConsumer(sourceId));
    if (source) {
        auto error = source->setOpenConfiguration(
                env,
                env->NewGlobalRef(thiz),
                surface);
        return fromConsumerError(error);
    }
    return JniSourceErrorType::SOURCE_NOT_FOUND;
}

namespace auvc::jni {

    void JniSurfaceConsumer::consume(const auvc::Frame &frame) {
        thread_local JniThreadAttacher attacher(g_jvm);
        JNIEnv *env = attacher.env;
        if (env == nullptr) return;

        jobject consumerObj = nullptr;
        {
            std::lock_guard<std::mutex> lock(jniConsumerMutex);
            if (consuming && jniConsumer != nullptr) {
                consumerObj = env->NewLocalRef(jniConsumer);
            }
        }

//        if (consumerObj != nullptr) {
//            jclass clazz = env->GetObjectClass(consumerObj);
//            jmethodID methodId = env->GetMethodID(clazz, "consume", "(Lcom/vsh/source/Frame;)V");
//            if (methodId != nullptr) {
//                env->CallVoidMethod(consumerObj, methodId, nullptr);
//            }
//            env->DeleteLocalRef(clazz);
//            env->DeleteLocalRef(consumerObj);
//        }
        if (nativeWindow != nullptr) {
            ANativeWindow_lock(nativeWindow, nullptr, nullptr);

            ANativeWindow_unlockAndPost(nativeWindow);

        }
    }

    JniSurfaceConsumer::JniSurfaceConsumer(JavaVM *g_jvm) :
            jniConsumer(nullptr), g_jvm(g_jvm) {

    }

    JniSurfaceConsumer::~JniSurfaceConsumer() {
        stopConsuming();
    }

    auvc::ConsumerError JniSurfaceConsumer::startConsuming() {
        std::lock_guard<std::mutex> lock(jniConsumerMutex);
        if (jniConsumer == nullptr) {
            return auvc::ConsumerError(auvc::ConsumerErrorCode::WRONG_CONFIGURATION,
                                       "JniSurfaceConsumer: jniConsumer is null");
        }
        if (nativeWindow == nullptr) {
            return auvc::ConsumerError(auvc::ConsumerErrorCode::WRONG_CONFIGURATION,
                                       "JniSurfaceConsumer: nativeWindow is null");
        }
        consuming = true;
        return auvc::ConsumerError::SUCCESS;
    }

    auvc::ConsumerError JniSurfaceConsumer::stopConsuming() {
        std::lock_guard<std::mutex> lock(jniConsumerMutex);
        consuming = false;
        if (jniConsumer != nullptr) {
            thread_local JniThreadAttacher attacher(g_jvm);
            JNIEnv *env = attacher.env;
            env->DeleteGlobalRef(jniConsumer);
            jniConsumer = nullptr;
            if (nativeWindow != nullptr) {
                ANativeWindow_release(nativeWindow);
                nativeWindow = nullptr;
            }
        }
        return auvc::ConsumerError::SUCCESS;
    }

    auvc::ConsumerError JniSurfaceConsumer::setOpenConfiguration(
            JNIEnv *env,
            jobject jniConsumer,
            jobject surface
    ) {
        std::lock_guard<std::mutex> lock(jniConsumerMutex);
        this->jniConsumer = jniConsumer;
        nativeWindow = ANativeWindow_fromSurface(env, surface);
        if (nativeWindow == nullptr) {
            return auvc::ConsumerError(auvc::ConsumerErrorCode::WRONG_CONFIGURATION,
                                       "JniSurfaceConsumer: failed to create native window");
        }
        return auvc::ConsumerError::SUCCESS;
    }

}
