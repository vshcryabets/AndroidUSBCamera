#include <catch2/catch_test_macros.hpp>
#include "TestSourceYUV420.h"
#include "u8x8.h"

TEST_CASE("Test source not ready before startCapture", "[TestSourceYUV420]") {
    TestSourceYUV420 source(u8x8_font_amstrad_cpc_extended_f);
    REQUIRE(source.isReadyForCapture() == false);
}

TEST_CASE("testFormatsAndResolutions", "[TestSourceYUV420]") {
    TestSourceYUV420 source(u8x8_font_amstrad_cpc_extended_f);
    source.open({});

    auto supportedFormats = source.getSupportedFrameFormats();
    auto supportedResolutions = source.getSupportedResolutions();

    REQUIRE(supportedFormats.size() == 1);
    REQUIRE(supportedFormats[0] == auvc::FrameFormat::YUV420P);

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
    source.startProducing(Source::CaptureConfiguration {
        .width = 640, 
        .height = 480,
        .fps = 30.0f
    });
    source.waitNextFrame();
    auto frame = source.readFrame();
    REQUIRE(frame.getWidth() == 640);
    REQUIRE(frame.getHeight() == 480);
    REQUIRE(frame.getFormat() == auvc::FrameFormat::YUV420P);
    REQUIRE(frame.getData() != nullptr);
    REQUIRE(frame.getSize() == 640*480 * 3 / 2); // YUV420P has 1.5 bytes per pixel

    source.stopProducing();
    source.close();
}

TEST_CASE("Test that source gives frames with delay", "[TestSourceYUV420]") {
    TestSourceYUV420 source(u8x8_font_amstrad_cpc_extended_f);
    source.open({});
    source.startProducing(Source::CaptureConfiguration {
        .width = 640, 
        .height = 480,
        .fps = 10.0f
    });
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    for (int i = 0; i < 10; i++) {
        source.waitNextFrame();
        auto frame = source.readFrame();
    }
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    // 10 frames at 10 fps should take at least 900 ms (some margin for scheduling)
    REQUIRE(duration >= 900);

    source.stopProducing();
    source.close();
}