#include <iostream>
#include <memory>
#include <algorithm>

#include "jni.h"
#include <android/log.h>

#include "jni/JniSurfaceConsumer.h"
#include "jni/JniSources.h"
#include "jni/JniSourcesRepo.h"
#include "jni/JniThreadAttacher.h"
#include "jni/JniObjectError.h"
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
JNIEXPORT jobject JNICALL
Java_com_vsh_source_SurfaceConsumer_nativeRelease(JNIEnv *env, jobject thiz, jint sourceId) {
    auto source = JniSourcesRepo::getInstance()->getConsumer(sourceId);
    if (source) {
        auto jniConsumer = std::dynamic_pointer_cast<JniSurfaceConsumer>(source);
        if (jniConsumer) {
            jniConsumer->stopConsuming();
        }
    } else {
        return fromConsumerError(env, auvc::ConsumerError::NOT_FOUND);
    }
    JniSourcesRepo::getInstance()->removeConsumer(sourceId);
    return fromConsumerError(env, auvc::ConsumerError::SUCCESS);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_vsh_source_SurfaceConsumer_nativeStopConsuming(JNIEnv *env, jobject thiz, jint sourceId) {
    auto source = std::dynamic_pointer_cast<JniSurfaceConsumer>(
            JniSourcesRepo::getInstance()->getConsumer(sourceId)
    );
    if (source) {
        return fromConsumerError(env, source->stopConsuming());
    }
    return fromConsumerError(env, auvc::ConsumerError::NOT_FOUND);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_vsh_source_SurfaceConsumer_nativeStartConsuming(
        JNIEnv *env,
        jobject thiz,
        jint sourceId) {
    auto source = std::dynamic_pointer_cast<JniSurfaceConsumer>(
            JniSourcesRepo::getInstance()->getConsumer(sourceId));
    if (source) {
        return fromConsumerError(env, source->startConsuming());
    }
    return fromConsumerError(env, auvc::ConsumerError::NOT_FOUND);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_vsh_source_SurfaceConsumer_nativeSetOpenConfiguration(
        JNIEnv *env,
        jobject thiz,
        jint sourceId,
        jobject surface,
        jint format,
        jint width,
        jint height) {
    auto source = std::dynamic_pointer_cast<JniSurfaceConsumer>(
            JniSourcesRepo::getInstance()->getConsumer(sourceId));
    if (source) {
        auto error = source->setOpenConfiguration(
                env,
                env->NewGlobalRef(thiz),
                surface,
                format,
                width,
                height);
        return fromConsumerError(env, error);
    }
    return fromConsumerError(env, auvc::ConsumerError::NOT_FOUND);
}

namespace auvc::jni {

    void JniSurfaceConsumer::consume(const auvc::Frame &frame) {
        {
            std::lock_guard<std::mutex> lock(jniConsumerMutex);
            if (!consuming) {
                return;
            }
        }
        thread_local JniThreadAttacher attacher(g_jvm);
        JNIEnv *env = attacher.env;
        if (env == nullptr) return;

        {
            std::lock_guard<std::mutex> lock(jniConsumerMutex);
            if (nativeWindow != nullptr) {
                ANativeWindow_Buffer buffer;
                if (ANativeWindow_lock(nativeWindow, &buffer, nullptr) == 0) {
                    auto *dest = (uint8_t *) buffer.bits;
                    const size_t bytes = buffer.width * 4; // Assuming RGBA_8888 format
                    const int dstStride = buffer.stride * 4;
                    const int32_t copyWidth = std::min(buffer.width, (int32_t) frame.getWidth());
                    const int32_t copyHeight = std::min(buffer.height, (int32_t) frame.getHeight());
                    uint8_t *frameBufferPos = frame.getData();
                    const int32_t srcStride = frame.getWidth() * 4; // Assuming RGBA_8888 format
                    for (int cy = 0; cy < copyHeight; cy++) {
                        memcpy(dest, frameBufferPos, copyWidth * 4); // Assuming RGBA_8888 format
                        dest += dstStride;
                        frameBufferPos += srcStride;
                    }
                    ANativeWindow_unlockAndPost(nativeWindow);
                }
            }
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
            if (env != nullptr)
                env->DeleteGlobalRef(jniConsumer);
            jniConsumer = nullptr;
        }
        if (nativeWindow != nullptr) {
            ANativeWindow_release(nativeWindow);
            nativeWindow = nullptr;
        }
        return auvc::ConsumerError::SUCCESS;
    }

    auvc::ConsumerError JniSurfaceConsumer::setOpenConfiguration(
            JNIEnv *env,
            jobject jniConsumer,
            jobject surface,
            jint format,
            jint width,
            jint height
    ) {
        std::lock_guard<std::mutex> lock(jniConsumerMutex);

        if (this->jniConsumer != nullptr) {
            env->DeleteGlobalRef(this->jniConsumer);
            this->jniConsumer = nullptr;
        }
        if (nativeWindow != nullptr) {
            ANativeWindow_release(nativeWindow);
            nativeWindow = nullptr;
        }

        this->jniConsumer = jniConsumer;
        nativeWindow = ANativeWindow_fromSurface(env, surface);
        if (nativeWindow == nullptr) {
            return auvc::ConsumerError(auvc::ConsumerErrorCode::WRONG_CONFIGURATION,
                                       "JniSurfaceConsumer: failed to create native window");
        }
        if (ANativeWindow_setBuffersGeometry(
                nativeWindow,
                width,
                height,
                WINDOW_FORMAT_RGBA_8888) != 0) {
            return auvc::ConsumerError(auvc::ConsumerErrorCode::WRONG_CONFIGURATION,
                                       "JniSurfaceConsumer: failed to set buffers geometry");
        }
        return auvc::ConsumerError::SUCCESS;
    }

}
