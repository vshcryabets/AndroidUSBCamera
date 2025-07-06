#include <catch2/catch_test_macros.hpp>
#include "Encoderx264.h"

TEST_CASE("testEncode", "[Encoderx264]") {
    X264Encoder encoder;
    X264Configuration config;
    config.width = 640;
    config.height = 480;
    config.fps_num = 30;
    config.fps_den = 1;
    config.keyframe_interval = 60; // Keyframe every 2 seconds at 30 fps
    config.level_idc = 30;         // H.264 Level 3.0
    config.profile = 0;
    config.annexb = true;        // Use Annex B format for NAL units
    config.intra_refresh = true; // Use IDR refresh
    config.crf = 23.0f;          // CRF value

    encoder.open(config);
    encoder.start();
    x264_picture_t *pic_in = encoder.getPicIn();
    int i = 0;
    for (int y = 0; y < config.height; ++y)
    {
        for (int x = 0; x < config.width; ++x)
        {
            pic_in->img.plane[0][y * pic_in->img.i_stride[0] + x] = (x + y + i * 5) % 256; // Y plane
        }
    }
    for (int y = 0; y < config.height / 2; ++y)
    {
        for (int x = 0; x < config.width / 2; ++x)
        {
            pic_in->img.plane[1][y * pic_in->img.i_stride[1] + x] = (x + y + i * 2) % 256; // U plane
            pic_in->img.plane[2][y * pic_in->img.i_stride[2] + x] = (x + y + i * 3) % 256; // V plane
        }
    }
    pic_in->i_pts = i; // Presentation timestamp for the frame
    EncoderMultiBuffer encoded = encoder.encodeFrame();

    REQUIRE(encoded.totalSize > 0);
    REQUIRE(encoded.buffers.size() > 0);
}