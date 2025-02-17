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
#include <stdint.h>

class GetJpegImageuseCase {
    public:
        virtual uint8_t* loadImage(std::string imageId) = 0;
};

class DecodeJpegImageUseCase {
    public:
        virtual void decodeImage(uint8_t* image, int size) = 0;
};

class JpegBenchmark {    
    public:
        JpegBenchmark();
        virtual ~JpegBenchmark();
        void start();
        void stop();
        void addFrame(int size);
}