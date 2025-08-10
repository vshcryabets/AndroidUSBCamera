#include <catch2/catch_test_macros.hpp>
#include "TestSourceYUV420.h"
#include "u8x8.h"

TEST_CASE("testFormatsAndResolutions", "[TestSourceYUV420]") {
    TestSourceYUV420 source(u8x8_font_amstrad_cpc_extended_f);
    source.open({});

    auto supportedFormats = source.getSupportedFrameFormats();
    auto supportedResolutions = source.getSupportedResolutions();

    REQUIRE(supportedFormats.size() == 1);
    REQUIRE(supportedFormats[0] == Source::FrameFormat::YUV420P);

    REQUIRE(supportedResolutions.size() == 1);
    auto firstResolution = supportedResolutions.begin();
    REQUIRE(firstResolution != supportedResolutions.end());
    uint16_t type = firstResolution->first;
    std::vector<Source::Resolution> resolutions = firstResolution->second;
    REQUIRE(resolutions.size() > 1);
    REQUIRE(resolutions[0].width == 640);
    REQUIRE(resolutions[0].height == 480);
    REQUIRE(resolutions[0].fps.size() > 1);
    REQUIRE(resolutions[0].fps[0] == 30.00f);

    source.close();
}

TEST_CASE("testCapture", "[TestSourceYUV420]") {
    TestSourceYUV420 source(u8x8_font_amstrad_cpc_extended_f);
    source.open({});
    source.startCapturing(Source::CaptureConfiguration {
        .width = 640, 
        .height = 480,
        .fps = 30.0f
    });
    source.waitNextFrame();
    auto frame = source.readFrame();
    REQUIRE(frame.width == 640);
    REQUIRE(frame.height == 480);
    REQUIRE(frame.format == Source::FrameFormat::YUV420P);
    REQUIRE(frame.data != nullptr);
    REQUIRE(frame.size == 640*480 * 3 / 2); // YUV420P has 1.5 bytes per pixel

    source.stopCapturing();
    source.close();
}
