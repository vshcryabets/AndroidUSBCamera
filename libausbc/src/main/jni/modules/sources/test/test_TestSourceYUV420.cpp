#include <catch2/catch_test_macros.hpp>
#include "TestSourceYUV420.h"
#include "u8x8.h"

TEST_CASE("Test source not ready before startCapture", "[TestSourceYUV420]") {
    TestSourceYUV420 source(u8x8_font_amstrad_cpc_extended_f);
    REQUIRE(source.isReadyForProducing() == false);
}

TEST_CASE("testFormatsAndResolutions", "[TestSourceYUV420]") {
    TestSourceYUV420 source(u8x8_font_amstrad_cpc_extended_f);
    source.open({});

    auto supportedFormats = source.getSupportedFrameFormats();
    auto supportedResolutions = source.getSupportedResolutions();

    REQUIRE(supportedFormats.size() == 1);
    REQUIRE(supportedFormats[0] == auvc::FrameFormat::YUV420P);

    REQUIRE(supportedResolutions.value().size() == 1);
    auto firstResolution = supportedResolutions.value().begin();
    REQUIRE(firstResolution != supportedResolutions.value().end());
    uint16_t type = firstResolution->first;
    std::vector<auvc::Resolution> resolutions = firstResolution->second;
    REQUIRE(resolutions.size() > 1);
    REQUIRE(resolutions[0].width == 640);
    REQUIRE(resolutions[0].height == 480);
    REQUIRE(resolutions[0].fps.size() > 1);
    REQUIRE(resolutions[0].fps[0] == 30.00f);

    source.close().get();
}

TEST_CASE("testCapture", "[TestSourceYUV420]") {
    TestSourceYUV420 source(u8x8_font_amstrad_cpc_extended_f);
    source.open({});
    source.startProducing(Source::ProducingConfiguration {
        .width = 640, 
        .height = 480,
        .fps = 30.0f
    }).get();
    source.waitNextFrame();
    auto frame = source.readFrame();
    REQUIRE(frame.has_value());
    auto frameValue = frame.value();
    REQUIRE(frameValue.getWidth() == 640);
    REQUIRE(frameValue.getHeight() == 480);
    REQUIRE(frameValue.getFormat() == auvc::FrameFormat::YUV420P);
    REQUIRE(frameValue.getData() != nullptr);
    REQUIRE(frameValue.getSize() == 640*480 * 3 / 2); // YUV420P has 1.5 bytes per pixel

    source.stopProducing().get();
    source.close().get();
}

TEST_CASE("Test that source gives frames with delay", "[TestSourceYUV420]") {
    TestSourceYUV420 source(u8x8_font_amstrad_cpc_extended_f);
    source.open({});
    source.startProducing(Source::ProducingConfiguration {
        .width = 640, 
        .height = 480,
        .fps = 10.0f
    }).get();
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    for (int i = 0; i < 10; i++) {
        source.waitNextFrame();
        auto frame = source.readFrame();
    }
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    // 10 frames at 10 fps should take at least 900 ms (some margin for scheduling)
    REQUIRE(duration >= 900);
    REQUIRE(duration < 1200); // but should not take too long
    source.stopProducing().get();
    source.close().get();
}