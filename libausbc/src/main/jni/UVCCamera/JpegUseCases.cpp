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
#include "JpegUseCases.h"
#include <algorithm>

#ifdef USE_LIBJPEG
    #include "jpeglib.h"
#endif
#ifdef USE_TURBOJPEG
    #include <turbojpeg.h>
#endif

LoadJpegImageUseCase::Result LoadJpegImageFromFileUseCase::load(std::string imageId) {
    FILE* file = fopen(imageId.c_str(), "rb");
    if (file == nullptr) {
        return LoadJpegImageUseCase::Result{nullptr, 0};
    }
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    auto* buffer = new uint8_t[size];
    fseek(file, 0, SEEK_SET);
    fread(buffer, 1, size, file);
    fclose(file);
    return LoadJpegImageUseCase::Result{buffer, size};
}

#ifdef USE_TURBOJPEG
DecodeJpegImageUseCase::Result DecodeJpegImageTurboJpegUseCase::decodeImage(uint8_t* encodedBuffer, 
    size_t encodedBufferSize, 
    uint8_t* decodedBuffer, 
    size_t decodedBufferSize) {
    // Decode image
    tjhandle handle = tjInitDecompress();
    int width, height, subsamp, colorspace;
    tjDecompressHeader3(handle, encodedBuffer, encodedBufferSize, &width, &height, &subsamp, &colorspace);
    tjDecompress2(handle, encodedBuffer, encodedBufferSize, decodedBuffer, width, 0, height, TJPF_RGB, 0);
    tjDestroy(handle);
    DecodeJpegImageUseCase::Result result = {
        .width = width,
        .height = height,
        .buffer = decodedBuffer,
        .size = (size_t)(width * height * 3),
        .timestamp = std::chrono::steady_clock::now()
    };
    return result;
}

std::string DecodeJpegImageTurboJpegUseCase::getDecoderName() {
    return "TurboJPEG (version unknown)";
}

DecodeJpegImageUseCase::Result
DecodeJpegImageTurboJpegUseCase::getImageInfo(uint8_t *encodedBuffer, size_t encodedBufferSize) {
    tjhandle handle = tjInitDecompress();
    int width, height, subsamp, colorspace;
    tjDecompressHeader3(handle, encodedBuffer, encodedBufferSize, &width, &height, &subsamp, &colorspace);
    tjDestroy(handle);
    DecodeJpegImageUseCase::Result result = {
            .width = width,
            .height = height,
            .buffer = nullptr,
            .size = (size_t)(width * height * 3),
            .timestamp = std::chrono::steady_clock::now()
    };
    return result;
}

#endif

#ifdef USE_LIBJPEG
DecodeJpegImageUseCase::Result DecodeJpegImageLibJpeg9UseCase::decodeImage(uint8_t* encodedBuffer, 
    size_t encodedBufferSize, 
    uint8_t* decodedBuffer, 
    size_t decodedBufferSize) {
    // Decode image
    jpeg_decompress_struct cinfo;
    jpeg_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_mem_src(&cinfo, encodedBuffer, encodedBufferSize);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    int row_stride = cinfo.output_width * cinfo.output_components;
    while (cinfo.output_scanline < cinfo.output_height) {
        uint8_t* buffer_array[1];
        buffer_array[0] = decodedBuffer + (cinfo.output_scanline) * row_stride;
        jpeg_read_scanlines(&cinfo, buffer_array, 1);
    }

    DecodeJpegImageUseCase::Result result = {
        .width = (int32_t)cinfo.output_width,
        .height = (int32_t)cinfo.output_height,
        .buffer = decodedBuffer,
        .size = cinfo.output_width * cinfo.output_height * cinfo.output_components,
        .timestamp = std::chrono::steady_clock::now()
    };

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    return result;
}

void DecodeJpegImageLibJpeg9UseCase::JPEGVersionError(j_common_ptr cinfo){}

std::string DecodeJpegImageLibJpeg9UseCase::getDecoderName() {
    jpeg_decompress_struct cinfo;
    jpeg_error_mgr error_mgr;
    error_mgr.error_exit = &JPEGVersionError;
    cinfo.err = &error_mgr;
    char *result = nullptr;
    jpeg_CreateDecompress(&cinfo, -1 /*version*/, sizeof(cinfo));
    int jpeg_version = cinfo.err->msg_parm.i[0];
    return "LibJPEG " + std::to_string(jpeg_version);
}

DecodeJpegImageUseCase::Result DecodeJpegImageLibJpeg9UseCase::getImageInfo(uint8_t* encodedBuffer, 
    size_t encodedBufferSize) {
    jpeg_decompress_struct cinfo;
    jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_mem_src(&cinfo, encodedBuffer, encodedBufferSize);
    jpeg_read_header(&cinfo, TRUE);

    DecodeJpegImageUseCase::Result result = {
        .width = (int32_t)cinfo.image_width,
        .height = (int32_t)cinfo.image_height,
        .buffer = nullptr,
        .size = cinfo.image_width * cinfo.image_height * cinfo.num_components,
        .timestamp = std::chrono::steady_clock::now()
    };
    jpeg_destroy_decompress(&cinfo);
    return result;
}
#endif

void SaveBitmapImageToFileUseCase::save(std::string imageId, uint8_t* buffer, size_t size) {
    FILE* file = fopen(imageId.c_str(), "wb");
    if (file == nullptr) {
        return;
    }
    fwrite(buffer, 1, size, file);
    fclose(file);
}
