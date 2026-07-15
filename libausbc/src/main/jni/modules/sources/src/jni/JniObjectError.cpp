#include "jni/JniObjectError.h"

namespace auvc::jni
{
    jobject fromConsumerError(
        JNIEnv* env,
        const auvc::ConsumerError &error
    )
    {
        jclass cls = env->FindClass("com/vsh/source/JniObjectError");
        if (cls == nullptr) {
            env->ThrowNew(env->FindClass("java/lang/RuntimeException"), "JniObjectError class not found");
            return nullptr; // Class not found
        }

        jmethodID constructor = env->GetMethodID(cls, "<init>", "(ILjava/lang/String;)V");
        if (constructor == nullptr) {
            env->ThrowNew(env->FindClass("java/lang/RuntimeException"), "JniObjectError constructor not found");
            return nullptr;
        }

        // convert to JniObjectErrorType.kt
        int code = 0;
        switch (error.getCode()) {
            case auvc::ConsumerErrorCode::SUCCESS:
                code = 0;
                break;
            case auvc::ConsumerErrorCode::WRONG_CONFIGURATION:
                code = 5;
                break;
            case auvc::ConsumerErrorCode::OBJECT_NOT_FOUND:
                code = 4;
                break;
            default:
                code = 1; // Unknown error
        }

        jstring message = env->NewStringUTF(error.what());
        jobject jniError = env->NewObject(
                cls,
                constructor,
                code,
                message
                );

        env->DeleteLocalRef(message);
        env->DeleteLocalRef(cls);
        return jniError;
    }


    jobject fromSourceError(
            JNIEnv* env,
            const auvc::SourceError &error
    )
    {
        jclass cls = env->FindClass("com/vsh/source/JniObjectError");
        if (cls == nullptr) {
            env->ThrowNew(env->FindClass("java/lang/RuntimeException"), "JniObjectError class not found");
            return nullptr; // Class not found
        }

        jmethodID constructor = env->GetMethodID(cls, "<init>", "(ILjava/lang/String;)V");
        if (constructor == nullptr) {
            env->ThrowNew(env->FindClass("java/lang/RuntimeException"), "JniObjectError constructor not found");
            return nullptr;
        }

        // convert to JniObjectErrorType.kt
        int code = 0;
        switch (error.getCode()) {
            case auvc::SourceErrorCode::SUCCESS:
                code = 0;
                break;
            case auvc::SourceErrorCode::SOURCE_ERROR_WRONG_CONFIG:
                code = 5;
                break;
            case auvc::SourceErrorCode::SOURCE_ERROR_CAPTURE_NOT_STARTED:
                code = 6;
                break;
            case auvc::SourceErrorCode::SOURCE_ERROR_NOT_OPENED:
                code = 3;
                break;
            case auvc::SourceErrorCode::SOURCE_ERROR_READ_AGAIN:
                code = 7;
                break;
            case auvc::SourceErrorCode::SOURCE_FRAME_NOT_AVAILABLE:
                code = 8;
                break;
            case auvc::SourceErrorCode::OBJECT_NOT_FOUND:
                code = 4;
                break;
            default:
                code = 1; // Unknown error
        }

        jstring message = env->NewStringUTF(error.what());
        jobject jniError = env->NewObject(
                cls,
                constructor,
                code,
                message
        );

        env->DeleteLocalRef(message);
        env->DeleteLocalRef(cls);
        return jniError;
    }
}
