#include <jni.h>
#include "TestSource.h"
#include "JniMappers.h"

extern "C" {

JNIEXPORT jlong JNICALL
Java_com_vsh_source_TestSource_nativeCreate(JNIEnv *env, jobject thiz, jlong fontPtr) {
    auto *source = new TestSource((const uint8_t*)fontPtr);//u8x8_font_amstrad_cpc_extended_f
    return reinterpret_cast<jlong>(source);
}

JNIEXPORT void JNICALL
Java_com_vsh_source_TestSource_nativeRelease(JNIEnv *env, jobject thiz, jlong ptr) {
    auto *camera = (TestSource *) ptr;
    delete camera;
}

JNIEXPORT void JNICALL
Java_com_vsh_source_TestSource_nativeStopCapturing(JNIEnv *env, jobject thiz, jlong ptr) {
    auto *camera = (TestSource *) ptr;
    camera->stopCapturing();
}

JNIEXPORT void JNICALL
Java_com_vsh_source_TestSource_nativeClose(JNIEnv *env, jobject thiz, jlong ptr) {
    auto *camera = (TestSource *) ptr;
    camera->close();
}

JNIEXPORT jobject JNICALL
Java_com_vsh_source_TestSource_nativeGetSupportedResolutions(JNIEnv *env,
                                                              jobject thiz,
                                                              jlong ptr) {
    auto *camera = (TestSource *) ptr;
    std::map<uint16_t, std::vector<Source::Resolution>> supportedSizes = camera->getSupportedResolutions();
    return resolutionMapToJObject(supportedSizes, env);
}
}
