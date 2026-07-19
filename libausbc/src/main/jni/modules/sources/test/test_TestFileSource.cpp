#include <catch2/catch_test_macros.hpp>
#include "TestFileSource.h"
#include <iostream>
#include <fstream>

TEST_CASE("testReadHeaders", "[TestFileSource]") {
    auvc::TestFileWriter writer("test.bin", 640, 480, "video/h264", 29.97f);
    writer.stopConsuming();

    auvc::TestFileSource source;
    source.open({
        .fileName = "test.bin"
    });
    auto supportedResolutions = source.getSupportedResolutions();
    REQUIRE(supportedResolutions.has_value());
    REQUIRE((*supportedResolutions).size() == 1);
    
    auto firstResolution = supportedResolutions.value().begin();
    REQUIRE(firstResolution != supportedResolutions.value().end());
    uint16_t type = firstResolution->first;
    auto resolutions = firstResolution->second;
    REQUIRE(resolutions.size() == 1);
    REQUIRE(resolutions[0].width == 640);
    REQUIRE(resolutions[0].height == 480);
    REQUIRE(resolutions[0].fps == 29.97f);
    REQUIRE(resolutions[0].frameFormat == auvc::FrameFormat::ENCODED);
    REQUIRE(source.getFramesCount() == 0);
}

TEST_CASE("testFramesReading", "[TestFileSource]") {
    auvc::TestFileSource source;
    source.open({
        .fileName = "testh264.bin"
    });
    REQUIRE(source.getFramesCount() == 60);
    auto supportedResolutions = source.getSupportedResolutions();
    REQUIRE(supportedResolutions.has_value());
    auto firstResolution = (*supportedResolutions).begin();
    REQUIRE(firstResolution != (*supportedResolutions).end());
    uint16_t type = firstResolution->first;
    auto resolutions = firstResolution->second;
    REQUIRE(resolutions.size() == 1);
    REQUIRE(resolutions[0].width == 640);
    REQUIRE(resolutions[0].height == 480);
    REQUIRE(resolutions[0].fps == 30.00f);

    source.startProducing({
        .width = 640,
        .height = 480,
        .fps = 30.0f
    }).get();
    REQUIRE(source.isReadyForProducing() == true);
    INFO("waitNextFrame should return true");
    REQUIRE(source.waitNextFrame() == true);


    REQUIRE(source.setCurrentFrame(10) == 10);
    for (size_t i = 0; i < 10 ; i++) {
        auto frame = source.readFrame();
        REQUIRE(frame.has_value());
        REQUIRE(frame.value().getData() != nullptr);
    }
    REQUIRE(source.getCurrentFrame() == 20);
}