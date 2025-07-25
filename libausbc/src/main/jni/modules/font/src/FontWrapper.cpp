#include "FontWrapper.h"
#include "u8x8.h"

extern "C"
JNIEXPORT jlong JNICALL
Java_com_vsh_font_FontSrcImpl_nativeGetFontPtr(JNIEnv *env, jobject thiz) {
    const uint8_t *font_ptr = u8x8_font_amstrad_cpc_extended_f;
    return reinterpret_cast<jlong>(font_ptr);
}

void FontWrapper_register(JNIEnv *env) {
    jclass clazz = env->FindClass("com/vsh/font/FontSrcImpl");
    if (clazz != nullptr) {
        static const JNINativeMethod methods[] = {
            {"nativeGetFontPtr", "()J", (void *) &Java_com_vsh_font_FontSrcImpl_nativeGetFontPtr},
        };
        env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0]));
    }
}