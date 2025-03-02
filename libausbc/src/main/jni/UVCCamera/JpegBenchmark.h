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
#pragma once

#include <string>
#include <cstdint>
#include <vector>
#include <utility>
#include <memory>
#include <functional>
#include <mutex>
#include <condition_variable>
#include "DI.h"

#ifdef USE_LIBJPEG
    #include "jpeglib.h"
#endif

class LoadJpegImageFromFileUseCase : public LoadJpegImageUseCase {
    public:
        Result load(std::string imageId) override;
};

class SaveBitmapImageToFileUseCase : public SaveBitmapImageUseCase {
    public:
        void save(std::string imageId, uint8_t* buffer, size_t size) override;
};

#ifdef USE_TURBOJPEG
class DecodeJpegImageTurboJpegUseCase : public DecodeJpegImageUseCase {
    public:
        void decodeImage(uint8_t* encodedBuffer, 
            size_t encodedBufferSize, 
            uint8_t* decodedBuffer, 
            size_t decodedBufferSize) override;
        std::string getDecoderName() override;
};
#endif

#ifdef USE_LIBJPEG
class DecodeJpegImageLibJpeg9UseCase : public DecodeJpegImageUseCase {
    private:
        static void JPEGVersionError(j_common_ptr cinfo);
    public:
        void decodeImage(uint8_t* encodedBuffer, 
            size_t encodedBufferSize, 
            uint8_t* decodedBuffer, 
            size_t decodedBufferSize) override;
        std::string getDecoderName() override;
};
#endif

template <typename T>
class ProgressObservable {
    protected:
        std::mutex mutex;
        std::condition_variable variable;
        bool done;
        T data;
    public:
        ProgressObservable(): done(false) {}
        void onComplete() {
            std::unique_lock<std::mutex> lock(mutex);
            done = true;
            variable.notify_all();
        }
        void setData(const T& newData) {
            std::unique_lock<std::mutex> lock(mutex);
            data = newData;
            variable.notify_all();
        }
        T getData() {
            std::unique_lock<std::mutex> lock(mutex);
            return data;
        }
        T wait() {
            std::unique_lock<std::mutex> lock(mutex);
            variable.wait(lock);
            return data;
        }
        bool isComplete() {
            std::unique_lock<std::mutex> lock(mutex);
            return done;
        }
        void subscribe(std::function<void(T)> callback) {
            std::unique_lock<std::mutex> lock(mutex);
            while (!done) {
                variable.wait(lock);
                callback(data);
            }
        }
};

struct JpegBenchmarkProgress {
    int sampleNumber;
    int iteration;
    std::chrono::milliseconds duration;
};

class JpegBenchmark {
    public:
        struct Arguments {
            std::vector<std::pair<uint16_t, std::string>> imageSamples;
            int iterations;
        };
    public:
        JpegBenchmark();
        virtual ~JpegBenchmark() {};
        std::shared_ptr<ProgressObservable<JpegBenchmarkProgress>> start(const Arguments& args);
};