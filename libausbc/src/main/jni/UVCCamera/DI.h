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
        virtual void decodeImage(uint8_t* encodedBuffer, 
            size_t encodedBufferSize, 
            uint8_t* decodedBuffer, 
            size_t decodedBufferSize) = 0;
        virtual std::string getDecoderName() = 0;
};

struct UseCases {
    LoadJpegImageUseCase* imageLoader;
    DecodeJpegImageUseCase* imageDecoder;
};

class DI {
    private:
        static DI* instance;    
    public:
        static DI* getInstance() { return instance; }
        static void setInstance(DI* instance) { DI::instance = instance; }
        virtual UseCases* getUseCases() = 0;
};