#include <jni.h>
#include "TestSource.h"
#include "TestSourceYUV420.h"
#include "JniSources.h"

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
        camera->stopCapturing();
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
            {"nativeCreate", "(J)J", (void *)&Java_com_vsh_source_TestSourceYUV420_nativeCreate},
            {"nativeGetSupportedFrameFormats", "(J)Ljava/util/List;", (void *)&Java_com_vsh_source_TestSourceYUV420_nativeGetSupportedFrameFormats},
            {"nativeOpen", "(J)V", (void *)&Java_com_vsh_source_TestSourceYUV420_nativeOpen},
            {"nativeGetSupportedResolutions", "(J)Ljava/util/Map;", (void *)&Java_com_vsh_source_TestSourceYUV420_nativeGetSupportedResolutions},
            {"nativeClose", "(J)V", (void *)&Java_com_vsh_source_TestSourceYUV420_nativeClose},
            {"nativeRelease", "(J)V", (void *)&Java_com_vsh_source_TestSourceYUV420_nativeRelease},
        };
        env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0]));
        env->DeleteLocalRef(clazz);
    }
}