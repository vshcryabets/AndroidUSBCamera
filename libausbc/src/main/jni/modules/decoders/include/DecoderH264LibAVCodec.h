#pragma once

#include "Decoder.h"
#include <x264.h>

struct X264DecoderConfig : public DecoderBaseConfiguration
{
};

class DecoderH264LibAVCodec : public Decoder, public DecoderWithConfiguration<X264DecoderConfig>
{
private:

public:
    DecoderH264LibAVCodec();
    virtual ~DecoderH264LibAVCodec();
    virtual void open(const X264DecoderConfig &config) override;
    void start() override;
    void stop() override;
    void close() override;
    DecoderMultiBuffer decodeFrame(const DecoderBuffer &inputBuffer) override;
};