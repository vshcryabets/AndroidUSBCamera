#pragma once

#include "Encoder.h"
#include <x264.h>

struct X264EncConfiguration : public EncoderBaseConfiguration
{
    uint16_t keyframe_interval; // Keyframe interval in seconds
    uint8_t level_idc;          // H.264 Level ID
    uint8_t profile;            // H.264 Profile (e.g., baseline, main, high)
    bool annexb;                // Use Annex B format for NAL units
    bool intra_refresh;         // Use IDR refresh
    float crf;                  // Constant Rate Factor (CRF) value for quality control
};

class X264Encoder : public Encoder, public EncoderWithConfiguration<X264EncConfiguration>
{
private:
    x264_param_t x264_param;
    x264_t *encoder = nullptr;
    x264_picture_t pic_in;
    x264_picture_t pic_out;

public:
    X264Encoder();
    virtual ~X264Encoder();
    virtual void open(const X264EncConfiguration &config) override;
    void start() override;
    void stop() override;
    void close() override;
    void consume(const auvc::Frame& frame) override;
    // x264_picture_t* getPicIn();
    // EncoderMultiBuffer encodeFrame() override;
    // EncoderMultiBuffer flush() override;
};