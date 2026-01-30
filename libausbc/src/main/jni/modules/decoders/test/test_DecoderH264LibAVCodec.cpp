#include <catch2/catch_test_macros.hpp>

#include <iostream>
#include <future>
#include <condition_variable>
#include <mutex>

#include "DecoderH264LibAVCodec.h"
#include "TestFileSource.h"
#include "PullToPushSource.h"

TEST_CASE("testDecode", "[DecoderH264LibAVCodec]")
{
    auto fileSource = std::make_shared<TestFileSource>();
    fileSource->open({.fileName = "framesFile.h264"});
    auto pullToPushSource = std::make_shared<PullToPushSource>();
    auto decoder = std::make_shared<DecoderH264LibAVCodec>();
    std::condition_variable cv;
    std::mutex cvMutex;
    int framesCounter = 10;

    X264DecoderConfig decoderOpenConfig;
    decoderOpenConfig.consumer = std::make_shared<auvc::ConsumerToFrameCallback>(
        [&](const auvc::Frame &frame) {
            std::cout << "Got frame number: " << framesCounter << std::endl;
            framesCounter--;
            if (framesCounter <= 0) {
                cv.notify_one();
            }
    });

    decoder->open(decoderOpenConfig);

    PullToPushSource::OpenConfiguration pullToPushSourceConfig;
    pullToPushSourceConfig.pullSource = fileSource;
    pullToPushSourceConfig.consumer = decoder;
    pullToPushSource->open(pullToPushSourceConfig);

    auto supportedResolutions = fileSource->getSupportedResolutions();
    REQUIRE(supportedResolutions.has_value());
    auto firstResolution = (*supportedResolutions).begin();
    REQUIRE(firstResolution != (*supportedResolutions).end());
    auto resolutions = firstResolution->second;

    fileSource->startProducing({
        .width = resolutions[0].width,
        .height = resolutions[0].height,
        .fps = resolutions[0].fps[0]
    }).get();
    decoder->startProducing({});
    pullToPushSource->startProducing({});

    std::unique_lock<std::mutex> lock(cvMutex);
    auto status = cv.wait_for(lock, std::chrono::seconds(5));
    REQUIRE(status == std::cv_status::no_timeout);
    REQUIRE(framesCounter <= 0);

    decoder->stopProducing().get();
    pullToPushSource->stopProducing().get();
    fileSource->stopProducing().get();

    pullToPushSource->close().get();
    decoder->close().get();
    fileSource->close().get();
}