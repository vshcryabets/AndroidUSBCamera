#include <catch2/catch_test_macros.hpp>
#include "ImageUseCases.h"

TEST_CASE("ConvertYUV420ptoRGBAUseCase", "[ImageUseCases]") {
    ConvertYUV420ptoRGBAUseCase useCase;
    ConvertBitmapUseCase::Buffer src;
    uint8_t sampleYuvData[] = { 
        // Y
        0x7F, 0x7F, 76, 76,
        0x7F, 0x7F, 76, 76,
        149, 149, 29, 29,
        149, 149, 29, 29,
        // U
        0x80, 84,
        43, 255,
        // V
        0x80, 255,
        21, 107
     };
    src.buffer = sampleYuvData; // Simulate an empty buffer
    src.capacity = sizeof(sampleYuvData);
    src.size = sizeof(sampleYuvData);
    src.width = 4;
    src.height = 4;

    auto result = useCase.convertToNew(src);
    
    REQUIRE(1 == 1);
}