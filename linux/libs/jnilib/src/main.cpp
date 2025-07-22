#include <jni.h>
#include "FontWrapper.h"
#include "JniSources.h"

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    FontWrapper_register(env);
    JniSources_register(env);

    return JNI_VERSION_1_6;
}