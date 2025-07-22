#include "JniSources.h"
#include "JniTestSource.h"

jobject resolutionMapToJObject(const std::map<uint16_t, std::vector<Source::Resolution>> &map, JNIEnv *env)
{
    jclass hashMapCls = env->FindClass("java/util/HashMap");
    jclass arrayListCls = env->FindClass("java/util/ArrayList");
    jclass integerClass = env->FindClass("java/lang/Integer");
    jclass floatClass = env->FindClass("java/lang/Float");
    jmethodID hashMapInit = env->GetMethodID(hashMapCls, "<init>", "()V");
    jmethodID hashMapPut = env->GetMethodID(hashMapCls, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
    jmethodID arrayListInit = env->GetMethodID(arrayListCls, "<init>", "()V");
    jmethodID arrayListAdd = env->GetMethodID(arrayListCls, "add", "(Ljava/lang/Object;)Z");
    jmethodID initInteger =  env->GetMethodID( integerClass, "<init>", "(I)V");
    jmethodID initFloat =  env->GetMethodID( floatClass, "<init>", "(F)V");
    jclass sourceResolutionCls = env->FindClass("com/jiangdg/uvc/SourceResolution");
    jmethodID sourceResolutionInit = env->GetMethodID(sourceResolutionCls, "<init>", "(IIILjava/util/List;)V");
    jobject result = env->NewObject(hashMapCls, hashMapInit);

    for (const auto &[type, resolutions]: map) {
        jobject key = env->NewObject(integerClass, initInteger, (jint)type);
        jobject resolutionsList = env->NewObject(arrayListCls, arrayListInit);
        for (const auto &it: resolutions) {
            // Create a list of intervals
            jobject fpsList = env->NewObject(arrayListCls, arrayListInit);

            for (const auto &fps: it.fps) {
                jobject intervalObject = env->NewObject(floatClass, initFloat, (jfloat)fps);
                env->CallBooleanMethod(fpsList, arrayListAdd, intervalObject);
                env->DeleteLocalRef(intervalObject);
            }

            // Create a resolution object
            auto resolution = env->NewObject(sourceResolutionCls,
                                             sourceResolutionInit,
                                             it.id,
                                             it.width,
                                             it.height,
                                             fpsList);
            env->CallBooleanMethod(resolutionsList, arrayListAdd, resolution);
            env->DeleteLocalRef(resolution);
            env->DeleteLocalRef(fpsList);
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
    return result;
}

void JniSources_register(JNIEnv *env) {
    JniTestSource_register(env);
}