#pragma once

#include "Decoder.h"

extern "C" {
    #include <libavcodec/avcodec.h>
}

struct X264DecoderConfig : public DecoderBaseConfiguration
{
};

class DecoderH264LibAVCodec : public Decoder,
                              public DecoderWithConfiguration<X264DecoderConfig>
{
private:
    AVCodecContext* codecContext {nullptr};
public:
    DecoderH264LibAVCodec();
    virtual ~DecoderH264LibAVCodec();
    virtual void open(const X264DecoderConfig &config) override;
    std::future<void> startProducing(const Source::ProducingConfiguration &config) override;
    void consume(const auvc::Frame &frame) override;
    void stopConsuming() override;
    std::future<void> stopProducing() override;
    std::future<void> close() override;
    auvc::ExpectedResolutions getSupportedResolutions() const override;
    std::vector<auvc::FrameFormat> getSupportedFrameFormats() const override;
};