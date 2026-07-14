#pragma once

#include <cstdint>
#include <jni.h>
#include "Consumer.h"
#include "Source.h"

namespace auvc::jni {

jobject fromConsumerError(
    JNIEnv* env,
    const auvc::ConsumerError &error
    );


jobject fromSourceError(
    JNIEnv* env,
    const auvc::SourceError &error
    );
}