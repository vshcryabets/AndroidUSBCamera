#include <catch2/catch_test_macros.hpp>
#include "DataTypes.h"

TEST_CASE("Frame can be assigned to another with same buffer, and data should be copied", "[Frame]") {
    uint8_t src1Data[] = {1, 2, 3, 4, 5, 6};
    uint8_t src2Data[] = {0 ,0, 0, 0, 0, 0};
    auvc::Frame frame1(640, 480, auvc::FrameFormat::RGB, src1Data, 6, std::chrono::high_resolution_clock::now());
    auvc::Frame frame2(320, 240, auvc::FrameFormat::YUV420P, src2Data, 6, std::chrono::high_resolution_clock::now());

    frame2 = frame1;

    REQUIRE(frame2.getWidth() == 640);
    REQUIRE(frame2.getHeight() == 480);
    REQUIRE(frame2.getFormat() == auvc::FrameFormat::RGB);
    REQUIRE(std::equal(src1Data, src1Data + 6, frame2.getData()));
}