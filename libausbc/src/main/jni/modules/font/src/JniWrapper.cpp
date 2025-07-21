#include "jni.h"
#include "u8x8.h"

extern "C"
JNIEXPORT jlong JNICALL
Java_com_vsh_font_FontSrcImpl_nativeGetFontPtr(JNIEnv *env, jobject thiz) {
    return reinterpret_cast<jlong>(u8x8_font_amstrad_cpc_extended_f);
}