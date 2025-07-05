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

class ConvertBitmapUseCase {
    public:
        class Exception : public std::exception {
            public:
                Exception(const char* message) : message(message) {}
                const char* what() const noexcept override { return message; }
            private:
                const char* message;
        };
        struct Buffer {
            uint8_t* buffer;
            size_t capacity;
            size_t size;
            uint32_t width;
            uint32_t height;
        };
    public:
        virtual Buffer convertToNew(const Buffer &src) = 0;
        virtual Buffer convert(Buffer &dst, const Buffer &src) = 0;
};

class ConvertRGBtoRGBAUseCase : public ConvertBitmapUseCase {
    public:
        Buffer convertToNew(const Buffer &src) override;
        Buffer convert(Buffer &dst, const Buffer &src) override;
};

class ConvertYUYVtoRGBAUseCase : public ConvertBitmapUseCase {
    public:
        Buffer convertToNew(const Buffer &src) override;
        Buffer convert(Buffer &dst, const Buffer &src) override;
};

class ConvertYUV420ptoRGBAUseCase : public ConvertBitmapUseCase {
    public:
        Buffer convertToNew(const Buffer &src) override;
        Buffer convert(Buffer &dst, const Buffer &src) override;
};