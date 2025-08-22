#pragma once

#include <jni.h>
#include <map>
#include <vector>
#include "Source.h"

jobject resolutionMapToJObject(
        const std::map<uint16_t, std::vector<Source::Resolution>> &map,
        JNIEnv *env);

jobject frameFormatsToJList(
    const std::vector<auvc::FrameFormat> &formats,
    JNIEnv *env);

void JniSources_register(JNIEnv *env);