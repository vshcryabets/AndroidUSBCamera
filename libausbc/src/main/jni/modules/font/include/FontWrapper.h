#pragma once
#include "jni.h"

extern "C"
JNIEXPORT jlong JNICALL
Java_com_vsh_font_FontSrcImpl_nativeGetFontPtr(JNIEnv *env, jobject thiz);

void FontWrapper_register(JNIEnv *env);