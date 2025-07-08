#include <catch2/catch_test_macros.hpp>
#include "FrameDataInjectUseCase.h"
#include <iostream>
#include <fstream>

TEST_CASE("get4bit", "[FrameDataInjectUseCaseRGBXImpl]") {
    FrameDataInjectUseCaseRGBXImpl useCase(8,8);
    char buffer[] = {(char)0xF0, 0x0F, (char)0xAA, 0x55};
    uint8_t bufferSize = sizeof(buffer);
    REQUIRE(useCase.get4bit(buffer, bufferSize, 0) == 0x0F);
    REQUIRE(useCase.get4bit(buffer, bufferSize, 4) == 0x00);
    REQUIRE(useCase.get4bit(buffer, bufferSize, 8) == 0x00);
    REQUIRE(useCase.get4bit(buffer, bufferSize, 12) == 0x0F);
    REQUIRE(useCase.get4bit(buffer, bufferSize, 16) == 0x0A);
    REQUIRE(useCase.get4bit(buffer, bufferSize, 20) == 0x0A);
    REQUIRE(useCase.get4bit(buffer, bufferSize, 24) == 0x05);
    REQUIRE(useCase.get4bit(buffer, bufferSize, 28) == 0x05);
}

TEST_CASE("get4bit must return 0", "[FrameDataInjectUseCaseRGBXImpl]") {
    FrameDataInjectUseCaseRGBXImpl useCase(8,8);
    char buffer[] = {(char)0x12};
    uint8_t bufferSize = sizeof(buffer);
    REQUIRE(useCase.get4bit(buffer, bufferSize, 0) == 0x01);
    REQUIRE(useCase.get4bit(buffer, bufferSize, 4) == 0x02);
    REQUIRE(useCase.get4bit(buffer, bufferSize, 8) == 0x00);
    REQUIRE(useCase.get4bit(buffer, bufferSize, 12) == 0x00);
}

TEST_CASE("setMiddleRgb", "[FrameDataInjectUseCaseRGBXImpl]") {
    FrameDataInjectUseCaseRGBXImpl useCase(8,8);

    Source::Frame frame;
    frame.width = 32;
    frame.size = frame.width * 32 * 4;
    frame.data = new uint8_t[frame.size];
    frame.format = Source::FrameFormat::RGBX;

    for (size_t i = 0; i < frame.size; ++i) { frame.data[i] = 0x00; }

    useCase.setMiddleRgb(frame, 0, 0, 0x123456);
    REQUIRE(0x123456 == useCase.getMiddleRgb(frame, 0, 0));
    REQUIRE(0x12 == frame.data[0]);
    REQUIRE(0x34 == frame.data[1]);
    REQUIRE(0x56 == frame.data[2]);

    useCase.setMiddleRgb(frame, 8, 0, 0xABCDEF);
    REQUIRE(0x123456 == useCase.getMiddleRgb(frame, 0, 0));
    REQUIRE(0xABCDEF == useCase.getMiddleRgb(frame, 8, 0));
}

TEST_CASE("getDataSize", "[FrameDataInjectUseCaseRGBXImpl]") {
    FrameDataInjectUseCaseRGBXImpl useCase(8,8);

    Source::Frame frame;
    frame.width = 128;
    frame.size = frame.width * 128 * 4;
    frame.data = new uint8_t[frame.size];
    frame.format = Source::FrameFormat::RGBX;

    for (size_t i = 0; i < frame.size; ++i) { frame.data[i] = 0x80; }

    // Test data
    uint8_t data[] = {0xCA, 0xFE, 0xBE, 0xEF};
    auto injectedFrame = useCase.injectData(frame, (char*)data, sizeof(data));
    REQUIRE(injectedFrame.data != nullptr);
    REQUIRE(injectedFrame.size == frame.size);
    REQUIRE(injectedFrame.format == Source::FrameFormat::RGBX);
    REQUIRE(injectedFrame.width == frame.width);

    REQUIRE(0x0040C0 == useCase.getMiddleRgb(injectedFrame, 0, 0));

    REQUIRE(useCase.getDataSize(injectedFrame, 0, 0) == sizeof(data));
}