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
    }

    env->DeleteLocalRef(cls);
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
    JniSourcesRepo::getInstance()->getSource(sourceId)->stopProducing();
}

JNIEXPORT void JNICALL
Java_com_vsh_source_TestSource_nativeClose(JNIEnv *env, jobject thiz, jint sourceId)
{
    JniSourcesRepo::getInstance()->getSource(sourceId)->close();
}

JNIEXPORT jobject JNICALL
Java_com_vsh_source_TestSource_nativeGetSupportedResolutions(JNIEnv *env,
                                                             jobject thiz,
                                                             jint sourceId)
{    
    auvc::ExpectedResolutions supportedSizes =
        JniSourcesRepo::getInstance()->getSource(sourceId)->getSupportedResolutions();
    return resolutionMapToJObject(supportedSizes, env);
}

#pragma region TestSourceYUV420

JNIEXPORT jint JNICALL
Java_com_vsh_source_TestSourceYUV420_nativeCreate(JNIEnv *env, jobject thiz, jlong fontPtr)
{
    return JniSourcesRepo::getInstance()->addSource(std::make_shared<TestSourceYUV420>((const uint8_t *)fontPtr));
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
    auvc::ExpectedResolutions supportedSizes = source->getSupportedResolutions();
    return resolutionMapToJObject(supportedSizes, env);
}

JNIEXPORT void JNICALL
Java_com_vsh_source_TestSourceYUV420_nativeStopProducing(
        JNIEnv *env,
        jobject thiz,
        jint sourceId)
{
    auto source = JniSourcesRepo::getInstance()->getSource(sourceId);
    source->stopProducing();
}

JNIEXPORT void JNICALL
Java_com_vsh_source_TestSourceYUV420_nativeClose(
    JNIEnv *env,
    jobject thiz,
    jint sourceId)
{
    auto source = JniSourcesRepo::getInstance()->getSource(sourceId);
    source->close();
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
    Source::ProducingConfiguration config = parseProducingConfiguration(producingConfiguration, env);
    source->startProducing(config);
}

JNIEXPORT jobject JNICALL
Java_com_vsh_source_TestSourceYUV420_nativeReadFrame(
        JNIEnv *env,
        jobject thiz,
        jint sourceId)
{
    auto source = std::dynamic_pointer_cast<TestSourceYUV420>(JniSourcesRepo::getInstance()->getSource(sourceId));
    auto frame = source->readFrame();
    return prepareJniFrame(frame, env);
}
}

void JniTestSource_register(JNIEnv *env)
{
    jclass clazz = env->FindClass("com/vsh/source/TestSource");
    if (clazz != nullptr)
    {
        static const JNINativeMethod methods[] = {
            {"nativeCreate", "(J)I", (void *)&Java_com_vsh_source_TestSource_nativeCreate},
            {"nativeRelease", "(I)V", (void *)&Java_com_vsh_source_TestSource_nativeRelease},
            {"nativeStopCapturing", "(I)V", (void *)&Java_com_vsh_source_TestSource_nativeStopCapturing},
            {"nativeClose", "(I)V", (void *)&Java_com_vsh_source_TestSource_nativeClose},
            {"nativeGetSupportedResolutions", "(I)Ljava/util/Map;", (void *)&Java_com_vsh_source_TestSource_nativeGetSupportedResolutions},
        };
        env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0]));
        env->DeleteLocalRef(clazz);
    }

    clazz = env->FindClass("com/vsh/source/TestSourceYUV420");
    if (clazz != nullptr)
    {
        static const JNINativeMethod methods[] = {
            {CONST_LITERAL("nativeCreate"), CONST_LITERAL("(J)I"),
                 (void *)&Java_com_vsh_source_TestSourceYUV420_nativeCreate},
            {CONST_LITERAL("nativeGetSupportedFrameFormats"), CONST_LITERAL("(I)Ljava/util/List;"),
                 (void *)&Java_com_vsh_source_TestSourceYUV420_nativeGetSupportedFrameFormats},
            {CONST_LITERAL("nativeOpen"), CONST_LITERAL("(I)V"),
                 (void *)&Java_com_vsh_source_TestSourceYUV420_nativeOpen},
            {CONST_LITERAL("nativeGetSupportedResolutions"), CONST_LITERAL("(I)Ljava/util/Map;"),
                 (void *)&Java_com_vsh_source_TestSourceYUV420_nativeGetSupportedResolutions},
            {CONST_LITERAL("nativeClose"), CONST_LITERAL("(I)V"),
                (void *)&Java_com_vsh_source_TestSourceYUV420_nativeClose},
            {CONST_LITERAL("nativeRelease"), CONST_LITERAL("(I)V"),
                (void *)&Java_com_vsh_source_TestSourceYUV420_nativeRelease},
            {CONST_LITERAL("nativeIsReadyForProducing"), CONST_LITERAL("(I)Z"),
                (void *)&Java_com_vsh_source_TestSourceYUV420_nativeIsReadyForProducing},
            {CONST_LITERAL("nativeStartProducing"), CONST_LITERAL("(ILcom/vsh/source/Source$ProducingConfiguration;)V"),
                (void *)&Java_com_vsh_source_TestSourceYUV420_nativeStartProducing},
            {CONST_LITERAL("nativeWaitNextFrame"), CONST_LITERAL("(I)Z"),
                (void *)&Java_com_vsh_source_TestSourceYUV420_nativeWaitNextFrame},
            {CONST_LITERAL("nativeReadFrame"), CONST_LITERAL("(I)Lcom/vsh/source/Frame;"),
                (void *)&Java_com_vsh_source_TestSourceYUV420_nativeReadFrame},
            {CONST_LITERAL("nativeStopProducing"), CONST_LITERAL("(I)V"),
                (void *)&Java_com_vsh_source_TestSourceYUV420_nativeStopProducing},
        };
        env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0]));
        env->DeleteLocalRef(clazz);
    }
}

