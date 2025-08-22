#include <catch2/catch_test_macros.hpp>
#include "PullToPushSource.h"
#include "TestSourceYUV420.h"
#include "u8x8.h"
#include <atomic>
#include <condition_variable>
#include <mutex>

TEST_CASE("testFormatsAndResolutions", "[PullToPushSource]") {
    std::atomic<bool> callbackCalled{false};
    std::mutex mtx;
    std::condition_variable cv;

    auto source = std::make_shared<TestSourceYUV420>(u8x8_font_amstrad_cpc_extended_f);
    REQUIRE(source->isPullSource());
    REQUIRE(!source->isPushSource());

    source->open({});

    const Source::CaptureConfiguration realConfig = {
        .width = 320,
        .height = 240,
        .fps = 30.0f
    };

    PullToPushSource pullToPushSource;
    PullToPushSource::OpenConfiguration config;
    config.pullSource = source;
    config.frameCallback = [&](const Source::Frame &frame) {
        REQUIRE(frame.width == realConfig.width);
        REQUIRE(frame.height == realConfig.height);
        REQUIRE(frame.size > 0);
        REQUIRE(frame.data != nullptr);
        REQUIRE(frame.format == Source::FrameFormat::YUV420P);
        {
            std::lock_guard<std::mutex> lock(mtx);
            callbackCalled = true;
        }
        cv.notify_one();
    };

    pullToPushSource.open(config);

    // pullToPush capture configuration doesn't matter, it uses the pull source's configuration
    source->startCapturing(realConfig);
    pullToPushSource.startCapturing({
        .width = 0,
        .height = 0,
        .fps = 0.0f
    });

    // Wait for callback (timeout after 2 seconds)
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait_for(lock, std::chrono::seconds(2), [&] { return callbackCalled.load(); });
    }
    REQUIRE(callbackCalled);
    pullToPushSource.stopCapturing();
    source->stopCapturing();
    pullToPushSource.close();
    source->close();
}
