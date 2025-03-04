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
    uint8_t* buffer = new uint8_t[size];
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
        .size = width * height * 3
    };
    return result;
}

std::string DecodeJpegImageTurboJpegUseCase::getDecoderName() {
    return "TurboJPEG (version unknown)";
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
        .size = cinfo.output_width * cinfo.output_height * cinfo.output_components
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
#endif

JpegBenchmark::JpegBenchmark() {

}

std::shared_ptr<ProgressObservable<JpegBenchmarkProgress>> JpegBenchmark::start(const Arguments& args) {
    std::shared_ptr<ProgressObservable<JpegBenchmarkProgress>> progress = std::make_shared<ProgressObservable<JpegBenchmarkProgress>>();

    std::thread decodeThread([args, &progress]() {
        std::vector<std::pair<uint16_t, LoadJpegImageUseCase::Result>> buffers(args.imageSamples.size());

        std::transform(args.imageSamples.begin(), args.imageSamples.end(), buffers.begin(), 
            [load=DI::getInstance()->getUseCases()->imageLoader](auto &it) { 
                return std::pair(it.first, load->load(it.second));
            }
        );
    
        size_t decodedBufferSize = 3840 * 2160 * 3;
        uint8_t *decodedBuffer4k = new uint8_t[decodedBufferSize];
        DecodeJpegImageUseCase* decode = DI::getInstance()->getUseCases()->imageDecoder;
        
        auto start = std::chrono::steady_clock::now();
        std::vector<std::pair<int, std::chrono::milliseconds>> results;
        for (auto& it : buffers) {
            progress->setData(JpegBenchmarkProgress{it.first, results, std::chrono::milliseconds(0)});
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
        progress->setData(JpegBenchmarkProgress{0, results, total_duration});

        delete[] decodedBuffer4k;
        
        for (auto& it : buffers) {
            delete[] it.second.buffer;
        }
        progress->onComplete();
    });

    decodeThread.detach();
    return progress;
}

void SaveBitmapImageToFileUseCase::save(std::string imageId, uint8_t* buffer, size_t size) {
    FILE* file = fopen(imageId.c_str(), "wb");
    if (file == nullptr) {
        return;
    }
    fwrite(buffer, 1, size, file);
    fclose(file);
}