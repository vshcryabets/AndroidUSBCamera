#include <iostream>
#include <jni.h>
#include "TestSource.h"
#include "TestSourceYUV420.h"
#include "jni/JniSources.h"
#include "jni/JniSourcesRepo.h"

Source::ProducingConfiguration parseProducingConfiguration(jobject object, JNIEnv* env) {
    Source::ProducingConfiguration config;
    jclass cls = env->GetObjectClass(object);

    jfieldID widthField = env->GetFieldID(cls, "width", "I");
    jfieldID heightField = env->GetFieldID(cls, "height", "I");
    jfieldID fpsField = env->GetFieldID(cls, "fps", "F");

    config.width = env->GetIntField(object, widthField);
    config.height = env->GetIntField(object, heightField);
    config.fps = env->GetFloatField(object, fpsField);
    env->DeleteLocalRef(cls);
    return config;
}

jobject prepareJniFrame(const auvc::Frame &frame, JNIEnv *env) {
    jobject result = nullptr;
    jclass cls = env->FindClass("com/vsh/source/BytePixelBufferFrame");
    if (cls != nullptr) {
        jmethodID constructor = env->GetMethodID(cls, "<init>",
                                                 "(IIIJLjava/nio/ByteBuffer;)V");
        if (constructor != nullptr) {
            jobject jPixelBuffer = nullptr;
            if (frame.getData() != nullptr && frame.getSize() > 0)
                jPixelBuffer = env->NewDirectByteBuffer(frame.getData(),
                                                        frame.getSize());
            result = env->NewObject(cls, constructor,
                                            frame.getWidth(),
                                            frame.getHeight(),
                                            frame.getFormat(),
                                            frame.getTimestamp().time_since_epoch().count(),
                                            jPixelBuffer);
        }
        env->DeleteLocalRef(cls);
    }
    return result;
}

extern "C"
{

JNIEXPORT jint JNICALL
Java_com_vsh_source_TestSource_nativeCreate(JNIEnv *env, jobject thiz, jlong fontPtr)
{
    return JniSourcesRepo::getInstance()->addSource(std::make_shared<TestSource>((const uint8_t *)fontPtr));
}

JNIEXPORT void JNICALL
Java_com_vsh_source_TestSource_nativeRelease(JNIEnv *env, jobject thiz, jint sourceId)
{
    JniSourcesRepo::getInstance()->removeSource(sourceId);
}

JNIEXPORT void JNICALL
Java_com_vsh_source_TestSource_nativeStopCapturing(JNIEnv *env, jobject thiz, jint sourceId)
{
    JniSourcesRepo::getInstance()->getSource(sourceId)->stopProducing().get();
}

JNIEXPORT void JNICALL
Java_com_vsh_source_TestSource_nativeClose(JNIEnv *env, jobject thiz, jint sourceId)
{
    JniSourcesRepo::getInstance()->getSource(sourceId)->close().get();
}

JNIEXPORT jobject JNICALL
Java_com_vsh_source_TestSource_nativeGetSupportedResolutions(JNIEnv *env,
                                                             jobject thiz,
                                                             jint sourceId)
{    
    std::map<uint16_t, std::vector<Source::Resolution>> supportedSizes =
        JniSourcesRepo::getInstance()->getSource(sourceId)->getSupportedResolutions();
    return resolutionMapToJObject(supportedSizes, env);
}

#pragma region TestSourceYUV420

JNIEXPORT jint JNICALL
Java_com_vsh_source_TestSourceYUV420_nativeCreate(
    JNIEnv *env,
    jobject thiz,
    jlong fontPtr)
{
    return JniSourcesRepo::getInstance()->
        addSource(std::make_shared<TestSourceYUV420>((const uint8_t *)fontPtr));
}

JNIEXPORT jobject JNICALL
Java_com_vsh_source_TestSourceYUV420_nativeGetSupportedFrameFormats(
    JNIEnv *env,
    jobject thiz,
    jint sourceId)
{
    auto source = JniSourcesRepo::getInstance()->getSource(sourceId);
    std::vector<auvc::FrameFormat> formats = source->getSupportedFrameFormats();
    return frameFormatsToJList(formats, env);
}

JNIEXPORT void JNICALL
Java_com_vsh_source_TestSourceYUV420_nativeOpen(
    JNIEnv *env,
    jobject thiz,
    jint sourceId)
{
    auto source = JniSourcesRepo::getInstance()->getSource(sourceId);
    source->open({

    });
}

JNIEXPORT jobject JNICALL
Java_com_vsh_source_TestSourceYUV420_nativeGetSupportedResolutions(JNIEnv *env,
                                                                   jobject thiz,
                                                                   jint sourceId)
{
    auto source = JniSourcesRepo::getInstance()->getSource(sourceId);
    std::map<uint16_t, std::vector<Source::Resolution>> supportedSizes =
        source->getSupportedResolutions();
    return resolutionMapToJObject(supportedSizes, env);
}

JNIEXPORT void JNICALL
Java_com_vsh_source_TestSourceYUV420_nativeStopProducing(
        JNIEnv *env,
        jobject thiz,
        jint sourceId)
{
    auto source = JniSourcesRepo::getInstance()->getSource(sourceId);
    source->stopProducing().get();
}

JNIEXPORT void JNICALL
Java_com_vsh_source_TestSourceYUV420_nativeClose(
    JNIEnv *env,
    jobject thiz,
    jint sourceId)
{
    auto source = JniSourcesRepo::getInstance()->getSource(sourceId);
    source->close().get();
}

JNIEXPORT void JNICALL
Java_com_vsh_source_TestSourceYUV420_nativeRelease(
    JNIEnv *env,
    jobject thiz,
    jint sourceId)
{
    JniSourcesRepo::getInstance()->removeSource(sourceId);
}

JNIEXPORT jboolean JNICALL
Java_com_vsh_source_TestSourceYUV420_nativeIsReadyForProducing(
        JNIEnv *env,
        jobject thiz,
        jint sourceId)
{
    auto source = JniSourcesRepo::getInstance()->getSource(sourceId);
    return source->isReadyForProducing();
}

JNIEXPORT jboolean JNICALL
Java_com_vsh_source_TestSourceYUV420_nativeWaitNextFrame(
        JNIEnv *env,
        jobject thiz,
        jint sourceId)
{
    auto source = std::dynamic_pointer_cast<TestSourceYUV420>(JniSourcesRepo::getInstance()->getSource(sourceId));
    return source->waitNextFrame();
}

JNIEXPORT void JNICALL
Java_com_vsh_source_TestSourceYUV420_nativeStartProducing(
        JNIEnv *env,
        jobject thiz,
        jint sourceId,
        jobject producingConfiguration)
{
    auto source = JniSourcesRepo::getInstance()->getSource(sourceId);
    Source::ProducingConfiguration config = parseProducingConfiguration(
            producingConfiguration, env);
    source->startProducing(config).get();
}

JNIEXPORT jobject JNICALL
Java_com_vsh_source_TestSourceYUV420_nativeReadFrame(
        JNIEnv *env,
        jobject thiz,
        jint sourceId)
{
    auto source = std::dynamic_pointer_cast<TestSourceYUV420>(
            JniSourcesRepo::getInstance()->getSource(sourceId));
    auto frame = source->readFrame();
    return prepareJniFrame(frame, env);
}

JNIEXPORT jboolean JNICALL
Java_com_vsh_source_TestSourceYUV420_nativeIsPullSource(
        JNIEnv *env,
        jobject thiz,
        jint sourceId)
{
    auto source = std::dynamic_pointer_cast<TestSourceYUV420>(
            JniSourcesRepo::getInstance()->getSource(sourceId));
    if (!source) {
        return JNI_FALSE;
    }
    return source->isPullSource() ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT jboolean JNICALL
Java_com_vsh_source_TestSourceYUV420_nativeIsPushSource(
        JNIEnv *env,
        jobject thiz,
        jint sourceId)
{
    auto source = std::dynamic_pointer_cast<TestSourceYUV420>(
            JniSourcesRepo::getInstance()->getSource(sourceId));
    if (!source) {
        return JNI_FALSE;
    }
    return source->isPushSource() ? JNI_TRUE : JNI_FALSE;
}
}