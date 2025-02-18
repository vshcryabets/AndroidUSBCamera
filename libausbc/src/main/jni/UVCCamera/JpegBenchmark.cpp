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
#include "JpegBenchmark.h"
#include <iostream>
#include <algorithm>

#ifdef USE_LIBJPEG
    #include "jpeglib.h"
#endif

LoadJpegImageUseCase::Result LoadJpegImageFromFileUseCase::load(std::string imageId) {
    FILE* file = fopen(imageId.c_str(), "rb");
    if (file == nullptr) {
        return LoadJpegImageUseCase::Result{nullptr, 0};
    }
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    uint8_t* buffer = new uint8_t[size];
    fseek(file, 0, SEEK_SET);
    fread(buffer, 1, size, file);
    fclose(file);
    return LoadJpegImageUseCase::Result{buffer, size};
}

#ifdef USE_TURBOJPEG
void DecodeJpegImageTurboJpegUseCase::decodeImage(uint8_t* encodedBuffer, 
    size_t encodedBufferSize, 
    uint8_t* decodedBuffer, 
    size_t decodedBufferSize) {
    // Decode image
}

std::string DecodeJpegImageTurboJpegUseCase::getDecoderName() {
    return "JPEG";
}
#endif

#ifdef USE_LIBJPEG
void DecodeJpegImageLibJpeg9UseCase::decodeImage(uint8_t* encodedBuffer, 
    size_t encodedBufferSize, 
    uint8_t* decodedBuffer, 
    size_t decodedBufferSize) {
    // Decode image
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
#endif

JpegBenchmark::JpegBenchmark() {

}

void JpegBenchmark::start(const Arguments& args) {

    std::vector<std::pair<uint16_t, LoadJpegImageUseCase::Result>> buffers(args.imageSamples.size());

    std::transform(args.imageSamples.begin(), args.imageSamples.end(), buffers.begin(), 
        [load=DI::getInstance()->getUseCases()->imageLoader](auto &it) { 
            return std::pair(it.first, load->load(it.second));
        }
    );
    
    size_t decodedBufferSize = 3840 * 2160 * 3;
    uint8_t *decodedBuffer4k = new uint8_t[decodedBufferSize];
    DecodeJpegImageUseCase* decode = DI::getInstance()->getUseCases()->imageDecoder;
    
    for (auto& it : buffers) {
        for (int i = 0; i < args.iterations; i++) {
            decode->decodeImage(it.second.buffer, it.second.size, decodedBuffer4k, decodedBufferSize);
        }
    }
    delete[] decodedBuffer4k;
    for (auto& it : buffers) {
        delete[] it.second.buffer;
    }
}
