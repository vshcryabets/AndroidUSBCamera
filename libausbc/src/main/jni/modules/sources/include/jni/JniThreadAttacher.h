#pragma once

#include "jni.h"

namespace auvc::jni {

class JniThreadAttacher {
public:
    JNIEnv* env = nullptr;
    JavaVM* g_jvm;

    JniThreadAttacher(JavaVM* g_jvm) : g_jvm(g_jvm) {
        int status = g_jvm->GetEnv((void**)&env, JNI_VERSION_1_6);

        if (status == JNI_EDETACHED) {
#ifdef __ANDROID__
            g_jvm->AttachCurrentThread(&env, nullptr);
#else
            g_jvm->AttachCurrentThread((void**)&env, nullptr);
#endif
            needsDetach = true;
        }
    }

    ~JniThreadAttacher() {
        if (needsDetach) {
            g_jvm->DetachCurrentThread();
        }
    }

private:
    bool needsDetach = false;
};

}