#include <iostream>
#include <memory>

#include "jni.h"

#include "jni/JniCountConsumer.h"
#include "jni/JniSources.h"
#include "jni/JniSourcesRepo.h"
#include "jni/JniThreadAttacher.h"
#include "Consumer.h"

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
Java_com_vsh_source_CountConsumer_nativeClose(JNIEnv *env, jobject thiz, jint sourceId) {
    auto source = std::dynamic_pointer_cast<JniCountConsumer>(JniSourcesRepo::getInstance()->getConsumer(sourceId));
    if (source) {
        source->closeConsumer();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_vsh_source_CountConsumer_nativeOpen(JNIEnv *env, jobject thiz, jint sourceId) {
    auto source = std::dynamic_pointer_cast<JniCountConsumer>(JniSourcesRepo::getInstance()->getConsumer(sourceId));
    if (source) {
        source->setOpenConfiguration(env->NewGlobalRef(thiz));
        source->openConsumer();
    }
}

void JniCountConsumer::consume(const auvc::Frame &frame) {
    thread_local JniThreadAttacher attacher(g_jvm);
    std::printf("ASD JniCountConsumer::consume: start\n");
    JNIEnv* env = attacher.env;
    if (env == nullptr) return;

    if (consuming) {
        frameCount++;

        jclass clazz = env->GetObjectClass(jniConsumer);
        jmethodID methodId = env->GetMethodID(clazz, "consume", "(Lcom/vsh/source/Frame;)V"); 
        
        if (methodId != nullptr) {
            env->CallVoidMethod(jniConsumer, methodId, NULL);
        }
        
        env->DeleteLocalRef(clazz);

        std::printf("ASD JniCountConsumer::consume: counter %d\n", frameCount.load());
    }
}

JniCountConsumer::JniCountConsumer(JavaVM* g_jvm): 
    jniConsumer(nullptr), g_jvm(g_jvm) {

}

JniCountConsumer::~JniCountConsumer() {
    closeConsumer();
}

auvc::ConsumerError JniCountConsumer::openConsumer() {
    if (jniConsumer == nullptr) {
        return auvc::ConsumerError(auvc::ConsumerErrorCode::WRONG_CONFIGURATION, "JniCountConsumer: jniConsumer is null");
    }
    consuming = true;
    return auvc::ConsumerError::SUCCESS;
}

auvc::ConsumerError JniCountConsumer::closeConsumer() {
    std::printf("ASD JniCountConsumer::closeConsumer: start\n");
    thread_local JniThreadAttacher attacher(g_jvm);
    JNIEnv* env = attacher.env;
    env->DeleteGlobalRef(jniConsumer);
    consuming = false;
    return auvc::ConsumerError::SUCCESS;
}

void JniCountConsumer::setOpenConfiguration(
    jobject jniConsumer
) {
    this->jniConsumer = jniConsumer;
}
