#include <jni.h>
#include "TestSource.h"
#include "TestSourceYUV420.h"
#include "jni/JniSources.h"

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

JNIEXPORT jlong JNICALL
Java_com_vsh_source_TestSource_nativeCreate(JNIEnv *env, jobject thiz, jlong fontPtr)
{
    auto *source = new TestSource((const uint8_t *)fontPtr);
    return reinterpret_cast<jlong>(source);
}

JNIEXPORT void JNICALL
Java_com_vsh_source_TestSource_nativeRelease(JNIEnv *env, jobject thiz, jlong ptr)
{
    auto *camera = (TestSource *)ptr;
    delete camera;
}

JNIEXPORT void JNICALL
Java_com_vsh_source_TestSource_nativeStopCapturing(JNIEnv *env, jobject thiz, jlong ptr)
{
    auto *camera = (TestSource *)ptr;
    camera->stopProducing();
}

JNIEXPORT void JNICALL
Java_com_vsh_source_TestSource_nativeClose(JNIEnv *env, jobject thiz, jlong ptr)
{
    auto *camera = (TestSource *)ptr;
    camera->close();
}

JNIEXPORT jobject JNICALL
Java_com_vsh_source_TestSource_nativeGetSupportedResolutions(JNIEnv *env,
                                                             jobject thiz,
                                                             jlong ptr)
{
    auto *camera = (TestSource *)ptr;
    std::map<uint16_t, std::vector<Source::Resolution>> supportedSizes =
        camera->getSupportedResolutions();
    return resolutionMapToJObject(supportedSizes, env);
}

#pragma region TestSourceYUV420

JNIEXPORT jlong JNICALL
Java_com_vsh_source_TestSourceYUV420_nativeCreate(JNIEnv *env, jobject thiz, jlong fontPtr)
{
    auto *source = new TestSourceYUV420((const uint8_t *)fontPtr);
    return reinterpret_cast<jlong>(source);
}

JNIEXPORT jobject JNICALL
Java_com_vsh_source_TestSourceYUV420_nativeGetSupportedFrameFormats(
    JNIEnv *env,
    jobject thiz,
    jlong itemPtr)
{
    auto *source = reinterpret_cast<TestSourceYUV420 *>(itemPtr);
    std::vector<auvc::FrameFormat> formats = source->getSupportedFrameFormats();
    return frameFormatsToJList(formats, env);
}

JNIEXPORT void JNICALL
Java_com_vsh_source_TestSourceYUV420_nativeOpen(
    JNIEnv *env,
    jobject thiz,
    jlong itemPtr)
{
    auto *source = reinterpret_cast<TestSourceYUV420 *>(itemPtr);
    source->open({

    });
}

JNIEXPORT jobject JNICALL
Java_com_vsh_source_TestSourceYUV420_nativeGetSupportedResolutions(JNIEnv *env,
                                                                   jobject thiz,
                                                                   jlong itemPtr)
{
    auto *camera = reinterpret_cast<TestSourceYUV420 *>(itemPtr);
    std::map<uint16_t, std::vector<Source::Resolution>> supportedSizes =
        camera->getSupportedResolutions();
    return resolutionMapToJObject(supportedSizes, env);
}

JNIEXPORT void JNICALL
Java_com_vsh_source_TestSourceYUV420_nativeStopProducing(
        JNIEnv *env,
        jobject thiz,
        jlong itemPtr)
{
    auto *source = reinterpret_cast<TestSourceYUV420 *>(itemPtr);
    source->stopProducing();
}

JNIEXPORT void JNICALL
Java_com_vsh_source_TestSourceYUV420_nativeClose(
    JNIEnv *env,
    jobject thiz,
    jlong itemPtr)
{
    auto *source = reinterpret_cast<TestSourceYUV420 *>(itemPtr);
    source->close();
}

JNIEXPORT void JNICALL
Java_com_vsh_source_TestSourceYUV420_nativeRelease(
    JNIEnv *env,
    jobject thiz,
    jlong itemPtr)
{
    auto *source = reinterpret_cast<TestSourceYUV420 *>(itemPtr);
    delete source;
}

JNIEXPORT jboolean JNICALL
Java_com_vsh_source_TestSourceYUV420_nativeIsReadyForProducing(
        JNIEnv *env,
        jobject thiz,
        jlong itemPtr)
{
    auto *source = reinterpret_cast<TestSourceYUV420 *>(itemPtr);
    return source->isReadyForProducing();
}

JNIEXPORT jboolean JNICALL
Java_com_vsh_source_TestSourceYUV420_nativeWaitNextFrame(
        JNIEnv *env,
        jobject thiz,
        jlong itemPtr)
{
    auto *source = reinterpret_cast<TestSourceYUV420 *>(itemPtr);
    return source->waitNextFrame();
}

JNIEXPORT void JNICALL
Java_com_vsh_source_TestSourceYUV420_nativeStartProducing(
        JNIEnv *env,
        jobject thiz,
        jlong itemPtr,
        jobject producingConfiguration)
{
    auto *source = reinterpret_cast<TestSourceYUV420 *>(itemPtr);
    Source::ProducingConfiguration config = parseProducingConfiguration(producingConfiguration, env);
    source->startProducing(config);
}

JNIEXPORT jobject JNICALL
Java_com_vsh_source_TestSourceYUV420_nativeReadFrame(
        JNIEnv *env,
        jobject thiz,
        jlong itemPtr)
{
    auto *source = reinterpret_cast<TestSourceYUV420 *>(itemPtr);
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
            {"nativeCreate", "(J)J", (void *)&Java_com_vsh_source_TestSource_nativeCreate},
            {"nativeRelease", "(J)V", (void *)&Java_com_vsh_source_TestSource_nativeRelease},
            {"nativeStopCapturing", "(J)V", (void *)&Java_com_vsh_source_TestSource_nativeStopCapturing},
            {"nativeClose", "(J)V", (void *)&Java_com_vsh_source_TestSource_nativeClose},
            {"nativeGetSupportedResolutions", "(J)Ljava/util/Map;", (void *)&Java_com_vsh_source_TestSource_nativeGetSupportedResolutions},
        };
        env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0]));
        env->DeleteLocalRef(clazz);
    }

    clazz = env->FindClass("com/vsh/source/TestSourceYUV420");
    if (clazz != nullptr)
    {
        static const JNINativeMethod methods[] = {
            {CONST_LITERAL("nativeCreate"), CONST_LITERAL("(J)J"),
                 (void *)&Java_com_vsh_source_TestSourceYUV420_nativeCreate},
            {CONST_LITERAL("nativeGetSupportedFrameFormats"), CONST_LITERAL("(J)Ljava/util/List;"),
                 (void *)&Java_com_vsh_source_TestSourceYUV420_nativeGetSupportedFrameFormats},
            {CONST_LITERAL("nativeOpen"), CONST_LITERAL("(J)V"),
                 (void *)&Java_com_vsh_source_TestSourceYUV420_nativeOpen},
            {CONST_LITERAL("nativeGetSupportedResolutions"), CONST_LITERAL("(J)Ljava/util/Map;"),
                 (void *)&Java_com_vsh_source_TestSourceYUV420_nativeGetSupportedResolutions},
            {CONST_LITERAL("nativeClose"), CONST_LITERAL("(J)V"), 
                (void *)&Java_com_vsh_source_TestSourceYUV420_nativeClose},
            {CONST_LITERAL("nativeRelease"), CONST_LITERAL("(J)V"), 
                (void *)&Java_com_vsh_source_TestSourceYUV420_nativeRelease},
            {CONST_LITERAL("nativeIsReadyForProducing"), CONST_LITERAL("(J)Z"), 
                (void *)&Java_com_vsh_source_TestSourceYUV420_nativeIsReadyForProducing},
            {CONST_LITERAL("nativeStartProducing"), CONST_LITERAL("(JLcom/vsh/source/Source$ProducingConfiguration;)V"), 
                (void *)&Java_com_vsh_source_TestSourceYUV420_nativeStartProducing},
            {CONST_LITERAL("nativeWaitNextFrame"), CONST_LITERAL("(J)Z"), 
                (void *)&Java_com_vsh_source_TestSourceYUV420_nativeWaitNextFrame},
            {CONST_LITERAL("nativeReadFrame"), CONST_LITERAL("(J)Lcom/vsh/source/Frame;"),
                (void *)&Java_com_vsh_source_TestSourceYUV420_nativeReadFrame},
            {CONST_LITERAL("nativeStopProducing"), CONST_LITERAL("(J)V"),
                (void *)&Java_com_vsh_source_TestSourceYUV420_nativeStopProducing},
        };
        env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0]));
        env->DeleteLocalRef(clazz);
    }
}

