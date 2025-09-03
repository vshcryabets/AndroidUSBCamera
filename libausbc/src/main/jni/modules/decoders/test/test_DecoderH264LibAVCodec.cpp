#include <catch2/catch_test_macros.hpp>
#include "DecoderH264LibAVCodec.h"
#include "TestFileSource.h"
#include "PullToPushSource.h"

TEST_CASE("testDecode", "[DecoderH264LibAVCodec]")
{
    auto fileSource = std::make_shared<TestFileSource>();
    fileSource->open({.fileName = "framesFile.h264"});
    auto pullToPushSource = std::make_shared<PullToPushSource>();
    auto decoder = std::make_shared<DecoderH264LibAVCodec>();

    X264DecoderConfig decoderOpenConfig;
    decoderOpenConfig.frameCallback = [&](const auvc::Frame &frame)
    {
        // Process the decoded frame
    };

    decoder->open(decoderOpenConfig);

    PullToPushSource::OpenConfiguration pullToPushSourceConfig;
    pullToPushSourceConfig.pullSource = fileSource;
    pullToPushSourceConfig.consumer = decoder;
    pullToPushSource->open(pullToPushSourceConfig);

    decoder->startProducing({});

    pullToPushSource->startProducing({});

    // decoder.setSource(fileSource);
    // decoder.start();

    // // Simulate decoding process
    // while (decoder.isDecoding()) {
    //     decoder.decodeFrame();
    // }

    // REQUIRE(decoder.getDecodedFramesCount() > 0);
}