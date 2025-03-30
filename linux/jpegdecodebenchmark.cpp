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

DI* DI::instance = nullptr;
class LinuxDi: public DI {
    private:
        LoadJpegImageFromFileUseCase imageLoader;
#if defined(USE_LIBJPEG)
        DecodeJpegImageLibJpeg9UseCase decoder;
#elif defined(USE_TURBOJPEG)
        DecodeJpegImageTurboJpegUseCase decoder;
#endif
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

int main(int argc, char* argv[]) {
    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        std::cout << "Usage: " << argv[0] << " [options]\n";
        std::cout << "Options:\n";
        std::cout << "  --help                Show this help message\n";
        std::cout << "  --sampleIterations N  Set the number of iterations for each sample (default: 30)\n";
        exit(0);
    }
    LinuxDi di;
    printf("Decoder %s\n", di.getUseCases()->imageDecoder->getDecoderName().c_str());
    int sampleIterations = 30;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--sampleIterations") == 0 && i + 1 < argc) {
            sampleIterations = atoi(argv[++i]);
        }
    }
    printf("Iteration on each sample - %d\n", sampleIterations);
    JpegBenchmark benchmark;
    JpegBenchmark::Arguments args = {
        .imageSamples = {
            { 10360, "./app/src/main/assets/jpeg_samples/sample1_0360.jpg" },
            { 10480, "./app/src/main/assets/jpeg_samples/sample1_0480.jpg" },
            { 10720, "./app/src/main/assets/jpeg_samples/sample1_0720.jpg" },
            { 11080, "./app/src/main/assets/jpeg_samples/sample1_1080.jpg" },
            { 11440, "./app/src/main/assets/jpeg_samples/sample1_1440.jpg" },
            { 12160, "./app/src/main/assets/jpeg_samples/sample1_2160.jpg" },
            { 20360, "./app/src/main/assets/jpeg_samples/sample2_0360.jpg" },
            { 20480, "./app/src/main/assets/jpeg_samples/sample2_0480.jpg" },
            { 20720, "./app/src/main/assets/jpeg_samples/sample2_0720.jpg" },
            { 21080, "./app/src/main/assets/jpeg_samples/sample2_1080.jpg" },
            { 21440, "./app/src/main/assets/jpeg_samples/sample2_1440.jpg" },
            { 22160, "./app/src/main/assets/jpeg_samples/sample2_2160.jpg" },
            { 30360, "./app/src/main/assets/jpeg_samples/sample3_0360.jpg" },
            { 30480, "./app/src/main/assets/jpeg_samples/sample3_0480.jpg" },
            { 30720, "./app/src/main/assets/jpeg_samples/sample3_0720.jpg" },
            { 31080, "./app/src/main/assets/jpeg_samples/sample3_1080.jpg" },
            { 31440, "./app/src/main/assets/jpeg_samples/sample3_1440.jpg" },
            { 32160, "./app/src/main/assets/jpeg_samples/sample3_2160.jpg" },
        },
        .iterations = sampleIterations
    };
    auto progress = benchmark.start(args);
    progress->subscribe(
        [](const auto &progress) {
            printf("\rSample id %d", progress.currentSampleNumber);
            fflush(stdout);
        },
        [sampleIterations](const auto &progress) {
            printf("\n\nComplete time %ld ms\n", progress.totalTime.count());
            for (auto& it : progress.results) {
                printf("Sample id %d, time %0.00f ms per sample\n", it.first, (float)it.second.count() / (float)sampleIterations );
            }
        });
    return 0;
}
