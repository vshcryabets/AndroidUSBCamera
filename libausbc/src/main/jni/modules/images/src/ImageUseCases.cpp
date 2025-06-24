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
#include "ImageUseCases.h"
#include <algorithm>

ConvertBitmapUseCase::Buffer ConvertRGBtoRGBAUseCase::convert(Buffer dst, const Buffer &src) {
    size_t pixelsCount = src.width * src.height;
    size_t rgbaSize = pixelsCount * 4;
    if (dst.capacity < rgbaSize) {
        throw ConvertBitmapUseCase::Exception("Destination buffer is too small");
    }
    dst.size = rgbaSize;
    for (size_t i = 0; i < pixelsCount; i ++) {
        dst.buffer[i * 4] = src.buffer[i*3];     // R
        dst.buffer[i * 4 + 1] = src.buffer[i*3 + 1]; // G
        dst.buffer[i * 4 + 2] = src.buffer[i*3 + 2]; // B
        dst.buffer[i * 4 + 3] = 255;     // A
    }
    return dst;
}

ConvertBitmapUseCase::Buffer ConvertRGBtoRGBAUseCase::convertToNew(const Buffer &src) {
    size_t pixelsCount = src.width * src.height;
    Buffer newBuffer = {
        .buffer = new uint8_t[pixelsCount * 4],
        .capacity = pixelsCount * 4,
        .size = 0,
        .width = src.width,
        .height = src.height
    };
    return convert(newBuffer, src);
}

ConvertBitmapUseCase::Buffer ConvertYUYVtoRGBAUseCase::convert(Buffer dst, const Buffer &src) {
    size_t pixelsCount = src.width * src.height;
    size_t rgbaSize = pixelsCount * 4;
    if (dst.capacity < rgbaSize) {
        throw ConvertBitmapUseCase::Exception("Destination buffer is too small");
    }
    size_t out_index = 0;
    dst.size = rgbaSize;
    for (size_t i = 0; i < pixelsCount * 2; i += 4) {
        uint8_t y0 = src.buffer[i + 0];
        uint8_t u  = src.buffer[i + 1];
        uint8_t y1 = src.buffer[i + 2];
        uint8_t v  = src.buffer[i + 3];

        int c0 = y0 - 16;
        int c1 = y1 - 16;
        int d = u - 128;
        int e = v - 128;

        int r = (298 * c0 + 409 * e + 128) >> 8;
        int g = (298 * c0 - 100 * d - 208 * e + 128) >> 8;
        int b = (298 * c0 + 516 * d + 128) >> 8;

        dst.buffer[out_index++] = r < 0 ? 0 : r > 255 ? 255 : r;
        dst.buffer[out_index++] = g < 0 ? 0 : g > 255 ? 255 : g;
        dst.buffer[out_index++] = b < 0 ? 0 : b > 255 ? 255 : b;
        dst.buffer[out_index++] = 255;

        r = (298 * c1 + 409 * e + 128) >> 8;
        g = (298 * c1 - 100 * d - 208 * e + 128) >> 8;
        b = (298 * c1 + 516 * d + 128) >> 8;

        dst.buffer[out_index++] = r < 0 ? 0 : r > 255 ? 255 : r;
        dst.buffer[out_index++] = g < 0 ? 0 : g > 255 ? 255 : g;
        dst.buffer[out_index++] = b < 0 ? 0 : b > 255 ? 255 : b;
        dst.buffer[out_index++] = 255;
    }
    return dst;
}

ConvertBitmapUseCase::Buffer ConvertYUYVtoRGBAUseCase::convertToNew(const Buffer &src) {
    size_t pixelsCount = src.width * src.height;
    Buffer newBuffer = {
        .buffer = new uint8_t[pixelsCount * 4],
        .capacity = pixelsCount * 4,
        .size = 0,
        .width = src.width,
        .height = src.height
    };
    return convert(newBuffer, src);
}


ConvertBitmapUseCase::Buffer ConvertYUV420ptoRGBAUseCase::convert(
    Buffer dst, 
    const Buffer &src) {
    size_t pixelsCount = src.width * src.height;
    size_t rgbaSize = pixelsCount * 4;
    if (dst.capacity < rgbaSize) {
        throw ConvertBitmapUseCase::Exception("Destination buffer is too small");
    }
    dst.size = rgbaSize;
    uint8_t* yBuffer = src.buffer;
    uint8_t* uBuffer = yBuffer + pixelsCount;
    uint8_t* vBuffer = uBuffer + (pixelsCount / 4);

    size_t out_index = 0;
    for (size_t i = 0; i < pixelsCount; i++) {
        uint8_t y = yBuffer[i];
        uint8_t u  = uBuffer[i / 4];
        uint8_t v  = vBuffer[i / 4];

        int c0 = y - 16;
        int d = u - 128;
        int e = v - 128;

        int r = (298 * c0 + 409 * e + 128) >> 8;
        int g = (298 * c0 - 100 * d - 208 * e + 128) >> 8;
        int b = (298 * c0 + 516 * d + 128) >> 8;

        dst.buffer[out_index++] = r < 0 ? 0 : r > 255 ? 255 : r;
        dst.buffer[out_index++] = g < 0 ? 0 : g > 255 ? 255 : g;
        dst.buffer[out_index++] = b < 0 ? 0 : b > 255 ? 255 : b;
        dst.buffer[out_index++] = 255;
    }
    return dst;
}

ConvertBitmapUseCase::Buffer ConvertYUV420ptoRGBAUseCase::convertToNew(const Buffer &src) {
    size_t pixelsCount = src.width * src.height;
    Buffer newBuffer = {
        .buffer = new uint8_t[pixelsCount * 4],
        .capacity = pixelsCount * 4,
        .size = 0,
        .width = src.width,
        .height = src.height
    };
    return convert(newBuffer, src);
}
