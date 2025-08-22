#include <catch2/catch_test_macros.hpp>
#include "TestFileSource.h"
#include <fstream>

TEST_CASE("testHeaders", "[TestFileWriter]") {
    TestFileWriter writer("test.bin", 640, 480, "video/h264", 29.97f);
    writer.stopConsuming();
    // Check if the file was created and has the correct headers
    std::ifstream file("test.bin", std::ios::binary);
    REQUIRE(file.is_open());
    char magic[4];
    file.read(magic, sizeof(magic));
    REQUIRE(magic[0] == 'A');
    REQUIRE(magic[1] == 'U');
    REQUIRE(magic[2] == 'V');
    REQUIRE(magic[3] == 'C');
    uint16_t width, height;
    file.read(reinterpret_cast<char*>(&width), sizeof(width));
    file.read(reinterpret_cast<char*>(&height), sizeof(height));
    REQUIRE(width == 640);
    REQUIRE(height == 480);
    float fps;
    file.read(reinterpret_cast<char*>(&fps), sizeof(fps));
    REQUIRE(fps == 29.97f);
    uint32_t toc;
    file.read(reinterpret_cast<char*>(&toc), sizeof(toc));
    REQUIRE(toc > 0); // Placeholder for TOC
    uint16_t mimeTypeSize;
    file.read(reinterpret_cast<char*>(&mimeTypeSize), sizeof(mimeTypeSize));
    REQUIRE(mimeTypeSize == 10); // "video/h264" has 10 characters
    // Read the mime type string
    std::string mimeType(mimeTypeSize, '\0');
    file.read(&mimeType[0], mimeTypeSize);
    REQUIRE(mimeType == "video/h264");
    file.close();
}

TEST_CASE("testWriteData", "[TestFileWriter]") {
    TestFileWriter writer("test.bin", 640, 480, "video/h264", 29.97f);
    uint8_t testData[1000] = {0x01, 0x02, 0x03, 0x04, 0x05};
    uint32_t framesCount = 160;
    auvc::Frame frame(640, 480, auvc::FrameFormat::ENCODED,
        testData,
        1000,
        std::chrono::high_resolution_clock::now()
    );
    for (int i = 0; i < framesCount; ++i) {
        writer.consume(frame);
    }
    writer.stopConsuming();

    // Check if the data was written correctly
    std::ifstream file("test.bin", std::ios::binary);
    REQUIRE(file.is_open());
    file.seekg(0, std::ios::end);
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    REQUIRE(fileSize > 0);
    char magic[4];
    file.read(magic, sizeof(magic));
    REQUIRE(magic[0] == 'A');
    REQUIRE(magic[1] == 'U');
    REQUIRE(magic[2] == 'V');
    REQUIRE(magic[3] == 'C');
    uint16_t width, height;
    float fps;
    file.read(reinterpret_cast<char*>(&width), sizeof(width));
    file.read(reinterpret_cast<char*>(&height), sizeof(height));
    file.read(reinterpret_cast<char*>(&fps), sizeof(fps));
    uint32_t toc;
    file.read(reinterpret_cast<char*>(&toc), sizeof(toc));
    // Read frames count
    uint32_t framesCountInToc;
    file.seekg(toc, std::ios::beg); // TOC position
    file.read(reinterpret_cast<char*>(&framesCountInToc), sizeof(framesCountInToc));
    REQUIRE(framesCountInToc == framesCount); // We wrote framesCount frames
    std::vector<uint32_t> framesTocItems(framesCountInToc);
    for (uint32_t i = 0; i < framesCountInToc; ++i) {
        file.read(reinterpret_cast<char*>(&framesTocItems[i]), sizeof(framesTocItems[i]));
    }
    for (uint32_t i = 1; i < framesCountInToc; ++i) {
        REQUIRE(framesTocItems[i] > framesTocItems[i - 1]);
    }
    // Check if the data was written correctly
    for (uint32_t i = 0; i < framesCountInToc; ++i) {
        uint32_t frameSize;
        file.seekg(framesTocItems[i], std::ios::beg);
        file.read(reinterpret_cast<char*>(&frameSize), sizeof(frameSize));
        REQUIRE(frameSize == 1000); // Each frame size should be 1000 bytes
    }
}