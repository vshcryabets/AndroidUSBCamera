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
#include "DI.h"

#ifdef USE_LIBJPEG
    #include "jpeglib.h"
#endif

class LoadJpegImageFromFileUseCase : public LoadJpegImageUseCase {
    public:
        Result load(std::string imageId) override;
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

class JpegBenchmark {
    public:
        struct Arguments {
            std::vector<std::pair<uint16_t, std::string>> imageSamples;
            int iterations;
        };
    public:
        JpegBenchmark();
        virtual ~JpegBenchmark() {};
        void start(const Arguments& args);
};