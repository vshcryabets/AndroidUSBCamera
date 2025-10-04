#include <catch2/catch_test_macros.hpp>
#include "Encoderx264.h"
#include "TestSourceYUV420.h"
#include <iostream>
#include "u8x8.h"
#include <fstream>
#include "TestFileSource.h"
#include <atomic>

TEST_CASE("testEncode", "[Encoderx264]") {
    uint32_t testWidth = 640;
    uint32_t testHeight = 480;
    float testFps = 30.0f;
    uint32_t testFrameSizeY = testWidth * testHeight;
    uint32_t testFrameSizeU = (testWidth / 2) * (testHeight / 2);
    TestSourceYUV420 source(u8x8_font_amstrad_cpc_extended_f);
    source.open({}); // Open the source
    source.startProducing({
        .width = testWidth,
        .height = testHeight,
        .fps = testFps
    }).get();


    X264Encoder encoder;
    X264EncConfiguration config;
    config.width = testWidth;
    config.height = testHeight;
    config.fps_num = (int)testFps;
    config.fps_den = 1;
    config.keyframe_interval = 30; // Keyframe every 1 second at 30 fps
    config.level_idc = 30;         // H.264 Level 3.0
    config.profile = 0;
    config.annexb = true;        // Use Annex B format for NAL units
    config.intra_refresh = true; // Use IDR refresh
    config.crf = 23.0f;          // CRF value


    TestFileWriter framesWriter("framesFile.h264", testWidth, testHeight, "video/h264", testFps);
    std::atomic<int> callbackCalled{0};
    config.frameCallback = [&framesWriter, &callbackCalled](const auvc::Frame &frame) {
        framesWriter.consume(frame);
        callbackCalled++;
        REQUIRE(frame.getSize() > 0);
        REQUIRE(frame.getData() != nullptr);
    };

    encoder.open(config);
    encoder.startProducing({}).get();


    for (uint32_t i = 0; i < 60; ++i) {
        auvc::ExpectedFrame expframe = source.readFrame(); // Read a new frame for each iteration
        if (!expframe.has_value()) {
            std::cerr << "Failed to read frame from source." << std::endl;
            break;
        }
        auvc::Frame frame = expframe.value();
    
        size_t requiredSize = testFrameSizeY + 2 * testFrameSizeU;
        REQUIRE(frame.getSize() >= requiredSize); // Ensure frame.data is large enough
        frame.setTimestamp(std::chrono::high_resolution_clock::now()); // Presentation timestamp for the frame
        encoder.consume(frame);
    }
    REQUIRE(callbackCalled.load() == 60);
    REQUIRE(framesWriter.getFramesCount() == 60);
    framesWriter.stopConsuming();
    source.stopProducing().get();
    source.close().get();
    encoder.stopProducing().get();
    encoder.close().get();
}