#include <catch2/catch_test_macros.hpp>
#include "PullToPushSource.h"
#include "TestSourceYUV420.h"
#include "u8x8.h"
#include <atomic>
#include <condition_variable>
#include <mutex>

TEST_CASE("testFormatsAndResolutions", "[TestSourceYUV420]") {
    std::atomic<bool> callbackCalled{false};
    std::mutex mtx;
    std::condition_variable cv;

    auto source = std::make_shared<TestSourceYUV420>(u8x8_font_amstrad_cpc_extended_f);
    REQUIRE(source->isPullSource());
    REQUIRE(!source->isPushSource());

    source->open({});
    PullToPushSource pullToPushSource;
    PullToPushSource::OpenConfiguration config;
    config.pullSource = source;
    config.frameCallback = [&](const Source::Frame &frame) {
        REQUIRE(frame.width > 0);
        REQUIRE(frame.height > 0);
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

    // Wait for callback (timeout after 2 seconds)
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait_for(lock, std::chrono::seconds(2), [&] { return callbackCalled.load(); });
    }
    REQUIRE(callbackCalled);

    source->close();
}
