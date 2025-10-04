#include <jni.h>
#include "FontWrapper.h"
#include "jni/JniSources.h"
#include <iostream>

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    std::cout << "JNI_OnLoad called" << std::endl;
    FontWrapper_register(env);
    JniSources_register(env);

    return JNI_VERSION_1_6;
}