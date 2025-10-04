#include "jni/JniCountConsumer.h"
#include "jni/JniSources.h"
#include "jni/JniSourcesRepo.h"
#include "Consumer.h"


extern "C"
JNIEXPORT void JNICALL
Java_com_vsh_source_CountConsumer_nativeOpen(
        JNIEnv *env,
        jobject thiz,
        jlong source_id,
        jstring tag) {
    // TODO: implement nativeOpen()
}

extern "C"
JNIEXPORT jlong JNICALL
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

void JniCountConsumer_register(JNIEnv *env) {
    JniCountConsumer *source = new JniCountConsumer();
    source->stopConsuming();
    delete source;
    jclass clazz = env->FindClass("com/vsh/source/CountConsumer");
    if (clazz != nullptr) {
        static const JNINativeMethod methods[] = {
                {CONST_LITERAL("nativeCreate"), CONST_LITERAL("()J"),
                 (void *) &Java_com_vsh_source_CountConsumer_nativeCreate},
                {CONST_LITERAL("nativeRelease"), CONST_LITERAL("(J)V"),
                    (void *) &Java_com_vsh_source_CountConsumer_nativeRelease},
                {CONST_LITERAL("nativeStopConsuming"), CONST_LITERAL("(J)V"),
                    (void *) &Java_com_vsh_source_CountConsumer_nativeStopConsuming},
        };
        env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0]));
        env->DeleteLocalRef(clazz);
    }
}

void JniCountConsumer::consume(const auvc::Frame &frame) {
    if (consuming) {
        frameCount++;
    }
}

void JniCountConsumer::stopConsuming() {
    consuming = false;
}