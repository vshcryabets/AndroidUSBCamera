#include <catch2/catch_test_macros.hpp>
#include "FrameDataInjectUseCase.h"
#include <iostream>
#include <fstream>

TEST_CASE("put4bit", "[FrameDataInjectUseCaseRGBXImpl]") {
    FrameDataInjectUseCaseRGBXImpl useCase(8,8);
    char buffer[] = {0, 0, 0, 0};
    useCase.put4bit(buffer, sizeof(buffer), 0, 0x01);
    useCase.put4bit(buffer, sizeof(buffer), 4, 0x02);
    useCase.put4bit(buffer, sizeof(buffer), 8, 0x03);
    useCase.put4bit(buffer, sizeof(buffer), 12, 0x04);
    useCase.put4bit(buffer, sizeof(buffer), 16, 0x05);
    useCase.put4bit(buffer, sizeof(buffer), 20, 0x06);
    useCase.put4bit(buffer, sizeof(buffer), 24, 0x07);
    useCase.put4bit(buffer, sizeof(buffer), 28, 0x08);
    REQUIRE(buffer[0] == 0x12);
    REQUIRE(buffer[1] == 0x34);
    REQUIRE(buffer[2] == 0x56);
    REQUIRE(buffer[3] == 0x78);
}

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

    auvc::Frame frame(32, 32, auvc::FrameFormat::RGBX);
    frame.size = frame.width * frame.height * 4;
    frame.data = new uint8_t[frame.size];
    for (size_t i = 0; i < frame.size; ++i) { frame.data[i] = 0x00; }

    useCase.setMiddleRgb(frame, 0, 0, 0x123456);
    REQUIRE(0x123456 == useCase.getMiddleRgb(frame, 0, 0));
    REQUIRE(0x12 == frame.data[0]);
    REQUIRE(0x34 == frame.data[1]);
    REQUIRE(0x56 == frame.data[2]);

    useCase.setMiddleRgb(frame, 8, 0, 0xABCDEF);
    useCase.setMiddleRgb(frame, 16, 0, 0x444444);
    useCase.setMiddleRgb(frame, 24, 0, 0x555555);
    REQUIRE(0x123456 == useCase.getMiddleRgb(frame, 0, 0));
    REQUIRE(0xABCDEF == useCase.getMiddleRgb(frame, 8, 0));
}

TEST_CASE("getDataSize", "[FrameDataInjectUseCaseRGBXImpl]") {
    FrameDataInjectUseCaseRGBXImpl useCase(8,8);

    auvc::Frame frame(256, 128, auvc::FrameFormat::RGBX);
    frame.size = frame.width * 128 * 4;
    frame.data = new uint8_t[frame.size];
    for (size_t i = 0; i < frame.size; ++i) { frame.data[i] = 0x80; }

    // Test data
    uint8_t data[] = {0xCA, 0xFE, 0xBE, 0xEF, 
        0x00, 0x01, 0x02, 0x03, 
        0x04, 0x05, 0x06, 0x07,
        0x08, 'A', 'B', 'C',
        'D', 'E', 'F', 'G',
        'H', 'I', 'J', 'K', 
        'L'};
    useCase.injectData(frame, (char*)data, sizeof(data));
    REQUIRE(0x1090C0 == (useCase.getMiddleRgb(frame, 0, 0) & 0xF0F0F0));
    REQUIRE(useCase.getDataSize(frame, 0, 0) == sizeof(data));
}

TEST_CASE("injectDataOverflowException", "[FrameDataInjectUseCaseRGBXImpl]") {
    FrameDataInjectUseCaseRGBXImpl useCase(8,8);

    auvc::Frame frame(128, 128, auvc::FrameFormat::RGBX);
    frame.size = frame.width * 128 * 4;
    frame.data = new uint8_t[frame.size];
    for (size_t i = 0; i < frame.size; ++i) { frame.data[i] = 0x80; }

    // Test data
    uint8_t data[] = {0xCA, 0xFE, 0xBE, 0xEF, 
        0x00, 0x01, 0x02, 0x03, 
        0x04, 0x05, 0x06, 0x07,
        0x08, 'A', 'B', 'C',
        'D', 'E', 'F', 'G',
        'H', 'I', 'J', 'K', 
        'L'};
    REQUIRE_THROWS_AS(useCase.injectData(frame, (char*)data, sizeof(data) + 1), std::overflow_error);
}

TEST_CASE("readData", "[FrameDataInjectUseCaseRGBXImpl]") {
    FrameDataInjectUseCaseRGBXImpl useCase(8,8);

    auvc::Frame frame(256, 128, auvc::FrameFormat::RGBX);
    frame.size = frame.width * 128 * 4;
    frame.data = new uint8_t[frame.size];
    for (size_t i = 0; i < frame.size; ++i) { frame.data[i] = 0x80; }

    // Test data
    const char *data = "Test data string, which is longer 16 bytes";
    uint16_t dataSize = strlen(data);
    // std::cout << "Data size: " << strlen(data) << std::endl;
    useCase.injectData(frame, (char*)data, dataSize);
    REQUIRE(useCase.getDataSize(frame, 0, 0) == dataSize);
    uint8_t newBuffer[128];
    useCase.readData(frame, 0, 0, (char*)newBuffer, sizeof(newBuffer));
    REQUIRE(dataSize == strlen((char*)newBuffer));
    REQUIRE(std::equal(data, data + dataSize, newBuffer));
}

TEST_CASE("setMiddleRgb", "[FrameDataInjectUseCaseYUV420pImpl]") {
    FrameDataInjectUseCaseYUV420pImpl useCase(8,8);

    auvc::Frame frame(32, 32, auvc::FrameFormat::YUV420P);
    frame.size = frame.width * frame.height + (frame.width * frame.height) / 2;
    frame.data = new uint8_t[frame.size];
    for (size_t i = 0; i < frame.size; ++i) { frame.data[i] = 0x80; }

    useCase.setMiddleRgb(frame, 0, 0, 0xA83858);
    REQUIRE(0xA03050 == (useCase.getMiddleRgb(frame, 0, 0) & 0xF0F0F0) );
    REQUIRE(0x5D == frame.data[0]);
    REQUIRE(0x5D == frame.data[1]);
    REQUIRE(0x5D == frame.data[2]);
    REQUIRE(0x7D == frame.data[frame.width * frame.height]);

    useCase.setMiddleRgb(frame, 8, 0, 0xA8C8E8);
    useCase.setMiddleRgb(frame, 16, 0, 0x444444);
    useCase.setMiddleRgb(frame, 24, 0, 0x555555);
    REQUIRE(0xA03050 == (useCase.getMiddleRgb(frame, 0, 0) & 0xF0F0F0));
    REQUIRE(0xA0C0E0 == (useCase.getMiddleRgb(frame, 8, 0) & 0xF0F0F0));
}

TEST_CASE("readData", "[FrameDataInjectUseCaseYUV420pImpl]") {
    FrameDataInjectUseCaseYUV420pImpl useCase(8,8);

    auvc::Frame frame(256, 128, auvc::FrameFormat::YUV420P);
    frame.size = frame.width * frame.height + (frame.width * frame.height) / 2;
    frame.data = new uint8_t[frame.size];
    for (size_t i = 0; i < frame.size; ++i) { frame.data[i] = 0x80; }

    // Test data
    const char *data = "Test";
    uint16_t dataSize = strlen(data);
    // std::cout << "Data size: " << strlen(data) << std::endl;
    useCase.injectData(frame, (char*)data, dataSize);
    REQUIRE(useCase.getDataSize(frame, 0, 0) == dataSize);
    uint8_t newBuffer[128];
    useCase.readData(frame, 0, 0, (char*)newBuffer, sizeof(newBuffer));
    REQUIRE(dataSize == strlen((char*)newBuffer));
    REQUIRE(std::equal(data, data + dataSize, newBuffer));
}