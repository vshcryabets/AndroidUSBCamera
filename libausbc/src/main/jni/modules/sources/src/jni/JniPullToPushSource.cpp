#include <iostream>

#include "jni/JniSources.h"
#include "PullToPushSource.h"
#include "jni/JniSourcesRepo.h"
#include "jni/JniObjectError.h"

using namespace auvc::jni;

extern "C"
JNIEXPORT void JNICALL
Java_com_vsh_source_PullToPushSource_nativeOpen(
        JNIEnv *env,
        jobject thiz,
        jint sourceId,
        jstring tag,
        jint pullSourceId,
        jint consumerId) {
    auto source = std::dynamic_pointer_cast<auvc::PullToPushSource>(JniSourcesRepo::getInstance()->getSource(sourceId));
    const char *native_tag = env->GetStringUTFChars(tag, nullptr);
    auto openConfig = auvc::PullToPushSource::OpenConfiguration();
    openConfig.pullSource = std::dynamic_pointer_cast<auvc::PullSource>(
            JniSourcesRepo::getInstance()->getSource(pullSourceId));
    openConfig.consumer = JniSourcesRepo::getInstance()->getConsumer(consumerId);
    source->open(openConfig);
    env->ReleaseStringUTFChars(tag, native_tag);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_vsh_source_PullToPushSource_nativeCreate(
        JNIEnv *env,
        jobject thiz) {
    return JniSourcesRepo::getInstance()->
            addSource(std::make_shared<auvc::PullToPushSource>());
}

extern "C"
JNIEXPORT void JNICALL
Java_com_vsh_source_PullToPushSource_nativeRelease(JNIEnv *env, jobject thiz, jint srcId) {
    JniSourcesRepo::getInstance()->removeSource(srcId);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_vsh_source_PullToPushSource_nativeStartProducing(
        JNIEnv *env,
        jobject thiz,
        jint sourceId
        )
{
    auto source = JniSourcesRepo::getInstance()->getSource(sourceId);
    if (source == nullptr) {
        return auvc::jni::fromSourceError(env, auvc::SourceError::NOT_FOUND);
    }
    source->startProducing(
            // there is no configuration for PullToPushSource, so we pass an empty ProducingConfiguration
            {
                    .width = 0,
                    .height = 0,
                    .fps = 0.0f
            }
    ).get();
    return auvc::jni::fromSourceError(env, auvc::SourceError::SUCCESS);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_vsh_source_PullToPushSource_nativeStopProducing(
        JNIEnv *env,
        jobject thiz,
        jint sourceId
)
{
    auto source = JniSourcesRepo::getInstance()->getSource(sourceId);
    if (source == nullptr) {
        return auvc::jni::fromSourceError(env, auvc::SourceError::NOT_FOUND);
    }
    JniSourcesRepo::getInstance()->getSource(sourceId)->stopProducing().get();
    return auvc::jni::fromSourceError(env, auvc::SourceError::SUCCESS);
}