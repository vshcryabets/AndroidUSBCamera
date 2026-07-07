#pragma once

#include <cstdint>
#include <jni.h>

enum JniSourceErrorType: jint {
    SUCCESS = 0,
    INVALID_ARGUMENT = 1,
    SOURCE_NOT_INITIALIZED = 2,
    SOURCE_NOT_FOUND = 3
};