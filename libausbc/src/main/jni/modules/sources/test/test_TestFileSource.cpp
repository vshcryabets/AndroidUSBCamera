#include <catch2/catch_test_macros.hpp>
#include "TestFileSource.h"
#include <iostream>
#include <fstream>

TEST_CASE("testReadHeaders", "[TestFileSource]") {
    TestFileWriter writer("test.bin", 640, 480, "video/h264", 29.97f);
    writer.finalize();

    TestFileSource source;
    source.open({
        .fileName = "test.bin"
    });
    auto supportedFormats = source.getSupportedFrameFormats();
    auto supportedResolutions = source.getSupportedResolutions();

    REQUIRE(supportedResolutions.size() == 1);
    REQUIRE(supportedFormats.size() == 1);
    REQUIRE(supportedFormats[0] == Source::FrameFormat::ENCODED);
    
    auto firstResolution = supportedResolutions.begin();
    REQUIRE(firstResolution != supportedResolutions.end());
    uint16_t type = firstResolution->first;
    std::vector<Source::Resolution> resolutions = firstResolution->second;
    REQUIRE(resolutions.size() == 1);
    REQUIRE(resolutions[0].width == 640);
    REQUIRE(resolutions[0].height == 480);
    REQUIRE(resolutions[0].fps.size() == 1);
    REQUIRE(resolutions[0].fps[0] == 29.97f);
    REQUIRE(source.getFramesCount() == 0);
}
