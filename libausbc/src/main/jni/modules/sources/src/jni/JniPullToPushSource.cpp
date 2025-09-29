#include <jni.h>
#include "JniSources.h"
#include "PullToPushSource.h"
#include "JniPullToPushSource.h"


void JniPullToPush_register(JNIEnv *env)
{
    jclass clazz = env->FindClass("com/vsh/source/PullToPushSource");
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