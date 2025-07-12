#include <catch2/catch_test_macros.hpp>
#include "ImageUseCases.h"

TEST_CASE("ConvertYUV420ptoRGBAUseCase", "[ImageUseCases]") {
    ConvertYUV420ptoRGBAUseCase useCase;
    ConvertBitmapUseCase::Buffer src;
    uint8_t sampleYuvData[] = { 
        // Y
        0x7F, 0x7F, 0x4C, 0x4C, // Grey, Red (255)
        0x7F, 0x7F, 0x4C, 0x4C,
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

TEST_CASE("ConvertYUV420ptoRGBAUseCase_greenBox", "[ImageUseCases]") {
    ConvertYUV420ptoRGBAUseCase useCase;
    ConvertBitmapUseCase::Buffer src;
    src.width = 8;
    src.height = 8;
    src.size = src.width * src.height * 3 / 2;
    src.buffer = new uint8_t[src.size];
    src.capacity = src.size;
    memset(src.buffer, 0x80, src.size); // Fill with zeroes
    uint16_t halfWidth = src.width / 2;
    uint8_t* testDataU = src.buffer + src.width * src.height; // U plane starts after Y plane
    uint8_t* testDataV = testDataU + (src.width * src.height) / 4; // V plane starts after U plane
    int boxSize = 2;

    for (int y = 0; y < boxSize; ++y)
    {
        for (int x = 0; x < boxSize; ++x)
        {
            testDataU[y * halfWidth + x] = 0x20; //(x + y + i * 2) % 256; // U plane
            testDataV[y * halfWidth + x] = 0x30; //(x + y + i * 3) % 256; // V plane
        }
    }

    auto result = useCase.convertToNew(src);
    REQUIRE(result.buffer != nullptr);
    REQUIRE(result.capacity == 8*8*4);
    REQUIRE(result.size == 8*8*4);
    REQUIRE(result.width == 8);
    REQUIRE(result.height == 8);

    for (int y = 0; y < src.width; ++y)
    {
        for (int x = 0; x < src.width; ++x)
        {
            int idx = y * result.width * 4 + x * 4;
            if (y < boxSize * 2 && x < boxSize * 2) {
                // green pixel check
                REQUIRE((int)result.buffer[idx + 3] == 255);
                REQUIRE(abs((int)result.buffer[idx + 0] - 3) < 2);
                REQUIRE(abs((int)result.buffer[idx + 1] - 0xE9) < 2);
                REQUIRE(abs((int)result.buffer[idx + 2] - 0) < 2);
            } else {
                // grey pixel check
                REQUIRE((int)result.buffer[idx + 3] == 255);
                REQUIRE(abs((int)result.buffer[idx + 0] - 0x82) < 3);
                REQUIRE(abs((int)result.buffer[idx + 1] - 0x82) < 3);
                REQUIRE(abs((int)result.buffer[idx + 2] - 0x82) < 3);
            }
        }
    }
}