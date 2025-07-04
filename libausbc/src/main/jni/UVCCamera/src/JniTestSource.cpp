#include <jni.h>
#include "TestSource.h"

extern "C"
JNIEXPORT jlong JNICALL
Java_com_jiangdg_uvc_TestSource_nativeCreate(JNIEnv *env, jobject thiz) {
    auto *source = new TestSource();
    return reinterpret_cast<jlong>(source);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_jiangdg_uvc_TestSource_nativeRelease(JNIEnv *env, jobject thiz, jlong ptr) {
    auto *camera = (TestSource*)ptr;
    delete camera;
}
extern "C"
JNIEXPORT void JNICALL
Java_com_jiangdg_uvc_TestSource_nativeStopCapturing(JNIEnv *env, jobject thiz, jlong ptr) {
    auto *camera = (TestSource*)ptr;
    camera->stopCapturing();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_jiangdg_uvc_TestSource_nativeClose(JNIEnv *env, jobject thiz, jlong ptr) {
    auto *camera = (TestSource*)ptr;
    camera->close();
}