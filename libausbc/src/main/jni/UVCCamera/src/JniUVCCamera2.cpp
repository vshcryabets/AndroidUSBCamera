#include <jni.h>
#include "UVCCamera2.h"

extern "C"
JNIEXPORT jlong JNICALL
Java_com_jiangdg_uvc_UVCCamera2_nativeCreate(JNIEnv *env, jobject thiz) {
    UVCCamera2 *camera = new UVCCamera2();
    return reinterpret_cast<jlong>(camera);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_jiangdg_uvc_UVCCamera2_nativeRelease(JNIEnv *env, jobject thiz, jlong ptr) {
    auto *camera = (UVCCamera2*)ptr;
    delete camera;
}
extern "C"
JNIEXPORT void JNICALL
Java_com_jiangdg_uvc_UVCCamera2_nativeStopCapturing(JNIEnv *env, jobject thiz, jlong ptr) {
    auto *camera = (UVCCamera2*)ptr;
    camera->stopProducing();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_jiangdg_uvc_UVCCamera2_nativeClose(JNIEnv *env, jobject thiz, jlong ptr) {
    auto *camera = (UVCCamera2*)ptr;
    camera->close();
}