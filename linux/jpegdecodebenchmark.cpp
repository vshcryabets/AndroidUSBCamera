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
#include <iostream>
#include <string.h>
#include <unistd.h>

#include "DI.h"
#include "JpegBenchmark.h"

class LinuxUseCases: public UseCases {
    private:
        LoadJpegImageFromFileUseCase imageLoader;
        DecodeJpegImageLibJpeg9UseCase decoder;
    public:
        LoadJpegImageUseCase* getImageLoader() { return &imageLoader; }
        DecodeJpegImageUseCase* getImageDecoder() { return &decoder; }
};

DI* DI::instance = nullptr;
class LinuxDi: public DI {
    private:
        LoadJpegImageFromFileUseCase imageLoader;
        DecodeJpegImageLibJpeg9UseCase decoder;
        SaveBitmapImageToFileUseCase imageSaver;

        UseCases useCases;
    public:
        LinuxDi(): useCases{
            .imageLoader = &imageLoader,
            .imageDecoder = &decoder,
            .imageSaver = &imageSaver
        } {
            DI::setInstance(this);
        }

        UseCases* getUseCases() {
            return &useCases;
        }
};

int main(void) {
    LinuxDi di;
    printf("Decoder %s\n", di.getUseCases()->imageDecoder->getDecoderName().c_str());
    JpegBenchmark benchmark;
    benchmark.start({
        .imageSamples = {
            { 360, "./app/src/main/assets/jpeg_samples/sample1_0360.jpg" },
            { 480, "./app/src/main/assets/jpeg_samples/sample1_0480.jpg" },
            { 720, "./app/src/main/assets/jpeg_samples/sample1_0720.jpg" },
            { 1080, "./app/src/main/assets/jpeg_samples/sample1_1080.jpg" },
            { 1440, "./app/src/main/assets/jpeg_samples/sample1_1440.jpg" },
            { 2160, "./app/src/main/assets/jpeg_samples/sample1_2160.jpg" },
        },
        .iterations = 1
    });

    return 0;
}
