#include <iostream>

#include "jni/JniPullToPushSource.h"
#include "jni/JniSources.h"
#include "PullToPushSource.h"
#include "jni/JniSourcesRepo.h"

extern "C"
JNIEXPORT void JNICALL
Java_com_vsh_source_PullToPushSource_nativeOpen(
        JNIEnv *env,
        jobject thiz,
        jlong source_ptr,
        jstring tag,
        jlong pull_source_ptr) {
    auto *source = reinterpret_cast<PullToPushSource*>(source_ptr);
    const char *native_tag = env->GetStringUTFChars(tag, nullptr);
    auto openConfig = PullToPushSource::OpenConfiguration();
    openConfig.pullSource = std::shared_ptr<PullSource>(reinterpret_cast<PullSource*>(pull_source_ptr));
    source->open(openConfig);
    env->ReleaseStringUTFChars(tag, native_tag);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_vsh_source_PullToPushSource_nativeCreate(
        JNIEnv *env,
        jobject thiz) {
    return JniSourcesRepo::getInstance()->
            addSource(std::make_shared<PullToPushSource>());
}

extern "C"
JNIEXPORT void JNICALL
Java_com_vsh_source_PullToPushSource_nativeRelease(JNIEnv *env, jobject thiz, jint srcId) {
    JniSourcesRepo::getInstance()->removeSource(srcId);
}

void JniPullToPushSource_register(JNIEnv *env) {
//    jclass clazz = env->FindClass("com/vsh/source/PullToPushSource");
//    if (clazz != nullptr) {
//        static const JNINativeMethod methods[] = {
//                {CONST_LITERAL("nativeCreate"), CONST_LITERAL("()I"),
//                 (void *) &Java_com_vsh_source_PullToPushSource_nativeCreate},
//                {CONST_LITERAL("nativeRelease"), CONST_LITERAL("(I)V"),
//                    (void *) &Java_com_vsh_source_PullToPushSource_nativeRelease},
//        };
//        int res = env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0]));
//        if (res < 0) {
//            std::cerr << "Failed to register native methods for TestSourceYUV420" << std::endl;
//            env->ThrowNew(env->FindClass("java/lang/RuntimeException"),
//                          "Failed to register native methods for TestSourceYUV420");
//        }
//        env->DeleteLocalRef(clazz);
//    }
}
