#pragma once

#include <jni.h>
#include <map>
#include <vector>
#include "Source.h"

#define CONST_LITERAL(X) const_cast<char*>(X)

namespace auvc::jni {

jobject resolutionMapToJObject(
        const auvc::ExpectedResolutions &map,
        JNIEnv *env);

jobject frameFormatsToJList(
    const std::vector<auvc::FrameFormat> &formats,
    JNIEnv *env);

}