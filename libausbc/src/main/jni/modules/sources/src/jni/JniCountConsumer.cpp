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
    JniSourcesRepo::getInstance()->removeConsumer(sourceId);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_vsh_source_CountConsumer_nativeStopConsuming(JNIEnv *env, jobject thiz, jint sourceId) {
    JniSourcesRepo::getInstance()->getConsumer(sourceId)->stopConsuming();
}

void JniCountConsumer::consume(const auvc::Frame &frame) {
    if (consuming) {
        frameCount++;
    }
}

void JniCountConsumer::stopConsuming() {
    consuming = false;
}