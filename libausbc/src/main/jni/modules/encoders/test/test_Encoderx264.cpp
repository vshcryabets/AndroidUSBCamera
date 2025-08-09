#include <catch2/catch_test_macros.hpp>
#include "Encoderx264.h"
#include "TestSourceYUV420.h"
#include <iostream>
#include "u8x8.h"
#include <fstream>
#include "TestFileSource.h"

TEST_CASE("testEncode", "[Encoderx264]") {
    uint32_t testWidth = 640;
    uint32_t testHeight = 480;
    float testFps = 30.0f;
    uint32_t testFrameSizeY = testWidth * testHeight;
    uint32_t testFrameSizeU = (testWidth / 2) * (testHeight / 2);
    TestSourceYUV420 source(u8x8_font_amstrad_cpc_extended_f);
    source.open({}); // Open the source
    source.startCapturing({
        .width = testWidth,
        .height = testHeight,
        .fps = testFps
    });

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

    encoder.open(config);
    encoder.start();
    x264_picture_t *pic_in = encoder.getPicIn();
    
    uint8_t singleBuffer[testFrameSizeY + 2 * testFrameSizeU];
    TestFileWriter framesWriter("framesFile.h264", testWidth, testHeight, "video/h264", testFps);

    for (uint32_t i = 0; i < 60; ++i) {
        Source::Frame frame = source.readFrame(); // Read a new frame for each iteration
        if (frame.data == nullptr) {
            std::cerr << "Failed to read frame from source." << std::endl;
            break;
        }
    
        size_t requiredSize = testFrameSizeY + 2 * testFrameSizeU;
        REQUIRE(frame.size >= requiredSize); // Ensure frame.data is large enough

        memcpy(pic_in->img.plane[0], frame.data, testFrameSizeY);
        memcpy(pic_in->img.plane[1], frame.data + testFrameSizeY, testFrameSizeU);
        memcpy(pic_in->img.plane[2], frame.data + testFrameSizeY + testFrameSizeU, testFrameSizeU);
        pic_in->i_pts = i; // Presentation timestamp for the frame
        EncoderMultiBuffer encoded = encoder.encodeFrame();

        uint32_t bufferPosition = 0;
        for (const auto& buf : encoded.buffers) {
            memcpy(singleBuffer + bufferPosition, buf.data, buf.size);
            bufferPosition += buf.size;
        }
        framesWriter.write(singleBuffer, bufferPosition);

        REQUIRE(encoded.totalSize > 0);
        REQUIRE(encoded.buffers.size() > 0);
    }
    framesWriter.finalize();
}