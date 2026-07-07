#pragma once

#include "jni.h"

class JniThreadAttacher {
public:
    JNIEnv* env = nullptr;
    JavaVM* g_jvm;

    JniThreadAttacher(JavaVM* g_jvm) : g_jvm(g_jvm) {
        int status = g_jvm->GetEnv((void**)&env, JNI_VERSION_1_6);

        if (status == JNI_EDETACHED) {
            g_jvm->AttachCurrentThread((void**)&env, nullptr);
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