#include <catch2/catch_test_macros.hpp>
#include "DecoderH264LibAVCodec.h"
#include "TestFileSource.h"

TEST_CASE("testDecode", "[DecoderH264LibAVCodec]") {
    TestFileSource fileSource;
    fileSource.open({
        .fileName = "framesFile.h264"}
    );

    // fileSource.readFrame(); // Read the first frame to initialize
    DecoderH264LibAVCodec decoder;

    //decoder.setSource(fileSource);
    //decoder.start();

    // // Simulate decoding process
    // while (decoder.isDecoding()) {
    //     decoder.decodeFrame();
    // }

    // REQUIRE(decoder.getDecodedFramesCount() > 0);
}