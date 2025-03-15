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

extern "C" {

JNIEXPORT void JNICALL
Java_com_vsh_uvc_JpegBenchmark_nativeStartBenchmark(JNIEnv *env,
                                                    jobject thiz,
                                                    jlong ptr,
                                                    jobject args) {
    JniJpegBenchmark *result = reinterpret_cast<JniJpegBenchmark *>(ptr);
    result->startAndSendToSockFd();
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
                                                     jobject thiz,
                                                     jstring socketFielPath) {
    const char* socketFilePathC = env->GetStringUTFChars(socketFielPath, nullptr);
    JniJpegBenchmark *result = new JniJpegBenchmark(socketFilePathC);
    return reinterpret_cast<jlong>(result);
}

JNIEXPORT void JNICALL
Java_com_vsh_uvc_JpegBenchmark_nativeDestroyBenchmark(JNIEnv *env, jobject thiz, jlong ptr) {
    JniJpegBenchmark *result = reinterpret_cast<JniJpegBenchmark *>(ptr);
    delete result;
}

JNIEXPORT jlong JNICALL
Java_com_vsh_uvc_JpegBenchmark_nativeGetSockFd(JNIEnv *env, jobject thiz, jlong ptr) {
    JniJpegBenchmark *result = reinterpret_cast<JniJpegBenchmark *>(ptr);
    return result->getSockFd();
}
}

JniJpegBenchmark::JniJpegBenchmark(const std::string &socketFilePath) {
    struct sockaddr_un addr;
    LOGD("ASD JniJpegBenchmark create socket %s", socketFilePath.c_str());
    // Create the socket
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockfd < 0) {
        LOGE("socket creation failed");
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, socketFilePath.c_str());

    // Remove previous socket file if exists
    unlink(socketFilePath.c_str());

    // Bind the socket
    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        LOGE("bind failed");
        close(sockfd);
    }

    // Start listening
    if (listen(sockfd, 5) < 0) {
        LOGE("listen failed");
        close(sockfd);
    }
    LOGD("ASD JniJpegBenchmark success");
}

JniJpegBenchmark::~JniJpegBenchmark() {
    close(sockfd);
}

int JniJpegBenchmark::getSockFd() const {
    return sockfd;
}

void JniJpegBenchmark::startAndSendToSockFd() {
    std::thread decodeThread([this]() {
        LOGD("ASD startAndSendToSockFd");
        std::string data = "";
        int counter = 0;
        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            data = std::to_string(counter);
            LOGD("ASD startAndSendToSockFd 2 %s", data.c_str());
            write(sockfd, data.c_str(), data.length());
            counter++;
        }
    });

    decodeThread.detach();
}
