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

JNIEXPORT void JNICALL
Java_com_vsh_uvc_JpegBenchmark_nativeStartBenchmark(JNIEnv *env,
                                                    jobject thiz,
                                                    jlong ptr,
                                                    jobject args,
                                                    jint writeFd) {
    auto *result = reinterpret_cast<JniJpegBenchmark *>(ptr);
    JpegBenchmark::Arguments argsForBenchmark;
//    argsForBenchmark.
    result->startAndSendToSockFd(writeFd, argsForBenchmark);
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

long JniJpegBenchmark::startAndSendToSockFd(int writeFd, const Arguments& args) {
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

//    std::thread decodeTestThread([this, progress]() {
//        LOGD("ASD startAndSendToSockFd");
//        JpegBenchmarkProgress data;
//        data.totalTime = std::chrono::milliseconds(0);
//        int counter = 100;
//        while (true) {
//            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//            LOGD("ASD startAndSendToSockFd 2 %d", counter);
//            data.currentSampleNumber = counter;
//            data.totalTime = std::chrono::milliseconds(counter * 237);
//            progress->setData(data, false);
//            counter++;
//        }
//    });
//    decodeTestThread.detach();

    std::thread decodeThread([args, progress]() {
        std::vector<std::pair<uint16_t, LoadJpegImageUseCase::Result>> buffers(args.imageSamples.size());

        std::transform(args.imageSamples.begin(), args.imageSamples.end(), buffers.begin(),
                       [load= DI::getInstance()->getUseCases()->imageLoader](auto &it) {
                           return std::pair(it.first, load->load(it.second));
                       }
        );

        size_t decodedBufferSize = 3840 * 2160 * 3;
        uint8_t *decodedBuffer4k = new uint8_t[decodedBufferSize];
        DecodeJpegImageUseCase* decode = DI::getInstance()->getUseCases()->imageDecoder;

        auto start = std::chrono::steady_clock::now();
        std::vector<std::pair<int, std::chrono::milliseconds>> results;
        for (auto& it : buffers) {
            progress->setData(JpegBenchmarkProgress{it.first, results, std::chrono::milliseconds(0)}, false);
            auto iteration_start = std::chrono::steady_clock::now();
            DecodeJpegImageUseCase::Result result;
            for (int i = 0; i < args.iterations; i++) {
                result = decode->decodeImage(
                        it.second.buffer,
                        it.second.size,
                        decodedBuffer4k,
                        decodedBufferSize);
            }
            auto iteration_end = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(iteration_end - iteration_start);
            results.push_back(std::pair(it.first, duration));
            // DI::getInstance()->getUseCases()->imageSaver->save(std::to_string(it.first) + ".data", result.buffer, result.size);
        }
        auto end = std::chrono::steady_clock::now();
        auto total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        progress->setData(JpegBenchmarkProgress{0, results, total_duration}, true);

        delete[] decodedBuffer4k;
        for (auto& it : buffers) {
            delete[] it.second.buffer;
        }
    });
    decodeThread.detach();

    return progress->getReadFd();
}
