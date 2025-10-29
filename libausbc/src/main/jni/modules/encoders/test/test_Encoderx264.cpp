#include <catch2/catch_test_macros.hpp>
#include "Encoderx264.h"
#include "TestSourceYUV420.h"
#include <iostream>
#include "u8x8.h"
#include <fstream>
#include "TestFileSource.h"
#include <atomic>

TEST_CASE("Encoder throws error if no consumer in open configuration", "[Encoderx264]") {
    X264Encoder encoder;
    X264EncConfiguration config;
    config.width = 123;
    config.height = 456;
    config.fps_num = 30;
    config.fps_den = 1;
    config.keyframe_interval = 30; // Keyframe every 1 second at 30 fps
    config.level_idc = 30;         // H.264 Level 3.0
    config.profile = 0;
    config.annexb = true;        // Use Annex B format for NAL units
    config.intra_refresh = true; // Use IDR refresh
    config.crf = 23.0f;          // CRF value

    REQUIRE_THROWS_AS(
        encoder.open(config),
        EncoderException
    );
}

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
    config.consumer = std::make_shared<auvc::ConsumerToFrameCallback>([&framesWriter, &callbackCalled](const auvc::Frame &frame) {
        framesWriter.consume(frame);
        callbackCalled++;
        REQUIRE(frame.getSize() > 0);
        REQUIRE(frame.getData() != nullptr);
    });

    encoder.open(config);
    encoder.startProducing({}).get();


    for (uint32_t i = 0; i < 60; ++i) {
        auvc::Frame frame = source.readFrame(); // Read a new frame for each iteration
        if (frame.getData() == nullptr) {
            std::cerr << "Failed to read frame from source." << std::endl;
            break;
        }
    
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