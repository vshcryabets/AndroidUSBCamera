#include "jni/JniPullToPushSource.h"
#include "jni/JniSources.h"
#include "PullToPushSource.h"

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
JNIEXPORT jlong JNICALL
Java_com_vsh_source_PullToPushSource_nativeCreate(
        JNIEnv *env,
        jobject thiz) {
    auto *source = new PullToPushSource();
    return reinterpret_cast<jlong>(source);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_vsh_source_PullToPushSource_nativeRelease(JNIEnv *env, jobject thiz, jlong ptr) {
    auto *source = reinterpret_cast<PullToPushSource*>(ptr);
    delete source;
}

void JniPullToPushSource_register(JNIEnv *env) {
    jclass clazz = env->FindClass("com/vsh/source/PullToPushSource");
    if (clazz != nullptr) {
        static const JNINativeMethod methods[] = {
                {CONST_LITERAL("nativeCreate"), CONST_LITERAL("()J"),
                 (void *) &Java_com_vsh_source_PullToPushSource_nativeCreate},
                {CONST_LITERAL("nativeRelease"), CONST_LITERAL("(J)V"),
                    (void *) &Java_com_vsh_source_PullToPushSource_nativeRelease},
        };
        env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0]));
        env->DeleteLocalRef(clazz);
    }
}
