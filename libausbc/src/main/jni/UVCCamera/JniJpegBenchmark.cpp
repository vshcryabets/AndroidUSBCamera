/*
 * Copyright 2025 vschryabets@gmail.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "JniJpegBenchmark.h"
#include <jni.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include "JpegBenchmark.h"
#include "utilbase.h"
#include <map>
#include <thread>
#include <arpa/inet.h>

extern "C" {

JNIEXPORT jlong JNICALL
Java_com_vsh_uvc_JpegBenchmark_nativeStartBenchmark(JNIEnv *env,
                                                    jobject thiz,
                                                    jlong ptr,
                                                    jobject args,
                                                    jint writeFd) {
    auto *result = reinterpret_cast<JniJpegBenchmark *>(ptr);
    return result->startAndSendToSockFd(writeFd);
}

JNIEXPORT void JNICALL
Java_com_vsh_uvc_JpegBenchmark_nativeCancelBenchmark(JNIEnv *env,
                                                     jobject thiz,
                                                     jlong ptr) {
    // TODO: implement nativeCancelBenchmark()
}

JNIEXPORT jobject JNICALL
Java_com_vsh_uvc_JpegBenchmark_nativeGetBenchamrkResults(JNIEnv *env, jobject thiz, jlong id) {
    // TODO: implement nativeGetBenchamrkResults()
}

JNIEXPORT jlong JNICALL
Java_com_vsh_uvc_JpegBenchmark_nativeCreateBenchmark(JNIEnv *env,
                                                     jobject thiz) {
    auto *result = new JniJpegBenchmark();
    return reinterpret_cast<jlong>(result);
}

JNIEXPORT void JNICALL
Java_com_vsh_uvc_JpegBenchmark_nativeDestroyBenchmark(JNIEnv *env, jobject thiz, jlong ptr) {
    auto *result = reinterpret_cast<JniJpegBenchmark *>(ptr);
    delete result;
}

}

JniJpegBenchmark::JniJpegBenchmark() {
}

JniJpegBenchmark::~JniJpegBenchmark() {
}

long JniJpegBenchmark::startAndSendToSockFd(int writeFd) {
    std::function<std::vector<char>(JpegBenchmarkProgress)> serializer = [](JpegBenchmarkProgress progress){
        std::ostringstream oss;
        uint32_t sampleNumber = htonl(progress.currentSampleNumber);
        uint16_t size = htons(progress.results.size());
        oss.write(reinterpret_cast<const char*>(&sampleNumber), sizeof(sampleNumber));
        oss.write(reinterpret_cast<const char*>(&size), sizeof(size));
        for (auto &result : progress.results) {
            int32_t sampleId = htonl(result.first);
            oss.write(reinterpret_cast<const char*>(&sampleId), sizeof(sampleId));
            uint64_t count = htonq(result.second.count());
            oss.write(reinterpret_cast<const char*>(&count), sizeof(count));
        }
        uint64_t count = htonq(progress.totalTime.count());
        oss.write(reinterpret_cast<const char*>(&count), sizeof(count));
        std::string str = oss.str();
        return std::vector<char>(str.begin(), str.end());
    };

    std::shared_ptr<ProgressObservablePipeImpl<JpegBenchmarkProgress>> progress =
            std::make_shared<ProgressObservablePipeImpl<JpegBenchmarkProgress>>(serializer, writeFd);

    std::thread decodeThread([this, progress]() {
        LOGD("ASD startAndSendToSockFd");
        JpegBenchmarkProgress data;
        data.totalTime = std::chrono::milliseconds(0);
        int counter = 100;
        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            LOGD("ASD startAndSendToSockFd 2 %d", counter);
            data.currentSampleNumber = counter;
            data.totalTime = std::chrono::milliseconds(counter * 237);
            progress->setData(data, false);
            counter++;
        }
    });
    decodeThread.detach();
    return progress->getReadFd();
}
