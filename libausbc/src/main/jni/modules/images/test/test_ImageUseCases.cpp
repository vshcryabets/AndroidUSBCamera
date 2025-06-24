#include <catch2/catch_test_macros.hpp>
#include "ImageUseCases.h"

TEST_CASE("ConvertYUV420ptoRGBAUseCase", "[ImageUseCases]") {
    ConvertYUV420ptoRGBAUseCase useCase;
    ConvertBitmapUseCase::Buffer src;
    uint8_t sampleYuvData[] = { 
        // Y
        0x7F, 0x7F, 76, 76, // Grey, Red (255)
        0x7F, 0x7F, 76, 76,
        149, 149, 29, 29, // Green, Blue
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
    REQUIRE(result.buffer != nullptr);
    REQUIRE(result.capacity == 16*4);
    REQUIRE(result.size == 16*4);
    REQUIRE(result.width == 4);
    REQUIRE(result.height == 4);
    // grey pixels
    REQUIRE((int)result.buffer[3] == 255);
    REQUIRE(abs((int)result.buffer[0] - 128) < 2); // Allow some tolerance for rounding
    REQUIRE(abs((int)result.buffer[1] - 128) < 2);
    REQUIRE(abs((int)result.buffer[2] - 128) < 2);

    // red pixels
    REQUIRE(abs((int)result.buffer[24] - 255) < 2);
    REQUIRE(abs((int)result.buffer[25] - 0) < 2);
    REQUIRE(abs((int)result.buffer[26] - 0) < 2);
    REQUIRE((int)result.buffer[27] == 255);

    // green pixels
    REQUIRE(abs((int)result.buffer[32] - 0) < 2);
    REQUIRE(abs((int)result.buffer[33] - 255) < 2);
    REQUIRE(abs((int)result.buffer[34] - 0) < 2);
    REQUIRE((int)result.buffer[35] == 255);

    // blue pixels
    REQUIRE(abs((int)result.buffer[56] - 0) < 2);
    REQUIRE(abs((int)result.buffer[57] - 0) < 2);
    REQUIRE(abs((int)result.buffer[58] - 255) < 2);
    REQUIRE((int)result.buffer[59] == 255);
}