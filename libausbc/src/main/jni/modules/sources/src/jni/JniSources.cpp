#include <jni.h>
#include <iostream>

#include "jni/JniSources.h"

namespace auvc::jni {

jobject resolutionMapToJObject(const auvc::ExpectedResolutions &map, JNIEnv *env)
{
    if (!map) {
        jclass exceptionClass = env->FindClass("java/lang/Exception");
        env->ThrowNew(exceptionClass, map.error().what());
        env->DeleteLocalRef(exceptionClass);
        return nullptr;
    }

    jclass hashMapCls = env->FindClass("java/util/HashMap");
    jclass arrayListCls = env->FindClass("java/util/ArrayList");
    jclass integerClass = env->FindClass("java/lang/Integer");
    jclass floatClass = env->FindClass("java/lang/Float");
    jclass pixelFormatClass = env->FindClass("com/jiangdg/uvc/PixelFormat");
    jmethodID hashMapInit = env->GetMethodID(hashMapCls, "<init>", "()V");
    jmethodID hashMapPut = env->GetMethodID(hashMapCls, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
    jmethodID arrayListInit = env->GetMethodID(arrayListCls, "<init>", "()V");
    jmethodID arrayListAdd = env->GetMethodID(arrayListCls, "add", "(Ljava/lang/Object;)Z");
    jmethodID initInteger =  env->GetMethodID( integerClass, "<init>", "(I)V");
    jmethodID initFloat =  env->GetMethodID( floatClass, "<init>", "(F)V");
    jmethodID fromValue = env->GetStaticMethodID(
        pixelFormatClass,
        "fromValue",
        "(I)Lcom/jiangdg/uvc/PixelFormat;"
    );
    jclass sourceResolutionCls = env->FindClass("com/jiangdg/uvc/SourceResolution");
    jmethodID sourceResolutionInit = env->GetMethodID(sourceResolutionCls, "<init>", "(IIIFLcom/jiangdg/uvc/PixelFormat;)V");
    jobject result = env->NewObject(hashMapCls, hashMapInit);

    for (const auto &[type, resolutions]: map.value()) {
        jobject key = env->NewObject(integerClass, initInteger, (jint)type);
        jobject resolutionsList = env->NewObject(arrayListCls, arrayListInit);
        for (const auto &it: resolutions) {
            // create pixel format
            jobject pixelFormat = env->CallStaticObjectMethod(
                pixelFormatClass,
                fromValue,
                static_cast<jint>(it.frameFormat)
            );
            // Create a resolution object
            auto resolution = env->NewObject(sourceResolutionCls,
                                             sourceResolutionInit,
                                             (jint)it.id,
                                             (jint)it.width,
                                             (jint)it.height,
                                             (jfloat)it.fps,
                                             pixelFormat);
            env->CallBooleanMethod(resolutionsList, arrayListAdd, resolution);
            env->DeleteLocalRef(resolution);
            env->DeleteLocalRef(pixelFormat);
        }

        env->CallObjectMethod(result, hashMapPut, key, resolutionsList);
        env->DeleteLocalRef(resolutionsList);
        env->DeleteLocalRef(key);
    }

    env->DeleteLocalRef(sourceResolutionCls);
    env->DeleteLocalRef(arrayListCls);
    env->DeleteLocalRef(hashMapCls);
    env->DeleteLocalRef(floatClass);
    env->DeleteLocalRef(integerClass);
    env->DeleteLocalRef(pixelFormatClass);
    return result;
}

// jobject frameFormatsToJList(const std::vector<auvc::FrameFormat> &formats, JNIEnv *env) {
//     jclass arrayListCls = env->FindClass("java/util/ArrayList");
//     jmethodID arrayListInit = env->GetMethodID(arrayListCls, "<init>", "()V");
//     jmethodID arrayListAdd = env->GetMethodID(arrayListCls, "add", "(Ljava/lang/Object;)Z");
//     jobject list = env->NewObject(arrayListCls, arrayListInit);
//     jclass integerClass = env->FindClass("java/lang/Integer");
//     jmethodID integerInit = env->GetMethodID(integerClass, "<init>", "(I)V");

//     for (const auto &format: formats) {
//         jobject formatObject = env->NewObject(integerClass, integerInit, (jint)format);
//         env->CallBooleanMethod(list, arrayListAdd, formatObject);
//         env->DeleteLocalRef(formatObject);
//     }

//     env->DeleteLocalRef(integerClass);
//     env->DeleteLocalRef(arrayListCls);
//     return list;
// }

}