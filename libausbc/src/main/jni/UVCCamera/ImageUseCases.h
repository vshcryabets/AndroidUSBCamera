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
#include <stdint.h>
#include <string>
#include <chrono>

#ifdef USE_LIBJPEG
    #include "jpeglib.h"
#endif

class LoadJpegImageUseCase {
    public:
        struct Result {
            uint8_t* buffer;
            size_t size;
        };
    public:
        virtual Result load(std::string imageId) = 0;
};

class DecodeJpegImageUseCase {
    public:
        struct Result {
            int32_t width;
            int32_t height;
            uint8_t* buffer;
            size_t size;
            std::chrono::time_point<std::chrono::steady_clock> timestamp;
        };
    public:
        virtual Result getImageInfo(uint8_t* encodedBuffer, 
            size_t encodedBufferSize) = 0;
        virtual Result decodeImage(uint8_t* encodedBuffer, 
            size_t encodedBufferSize, 
            uint8_t* decodedBuffer, 
            size_t decodedBufferSize) = 0;
        virtual std::string getDecoderName() = 0;
};

class SaveBitmapImageUseCase {
    public:
        virtual void save(std::string imageId, uint8_t* buffer, size_t size) = 0;
};

class ConvertBitmapUseCase {
    public:
        struct Result {
            uint8_t* buffer;
            size_t size;
            uint32_t width;
            uint32_t height;
        };
    public:
        virtual Result convert(uint8_t* buffer, size_t size, uint32_t width, uint32_t height) = 0;
};

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
        Result decodeImage(uint8_t* encodedBuffer, 
            size_t encodedBufferSize, 
            uint8_t* decodedBuffer, 
            size_t decodedBufferSize) override;
        std::string getDecoderName() override;
        Result getImageInfo(uint8_t* encodedBuffer,
                        size_t encodedBufferSize) override;
};
#endif

#ifdef USE_LIBJPEG
class DecodeJpegImageLibJpeg9UseCase : public DecodeJpegImageUseCase {
    private:
        static void JPEGVersionError(j_common_ptr cinfo);
    public:
        Result decodeImage(uint8_t* encodedBuffer, 
            size_t encodedBufferSize, 
            uint8_t* decodedBuffer, 
            size_t decodedBufferSize) override;
        std::string getDecoderName() override;
        Result getImageInfo(uint8_t* encodedBuffer, 
            size_t encodedBufferSize) override;
};
#endif

class ConvertRGBtoRGBAUseCase : public ConvertBitmapUseCase {
    public:
        Result convert(uint8_t* buffer, size_t size, uint32_t width, uint32_t height) override;
};