#pragma once

#include "Decoder.h"

struct X264DecoderConfig : public DecoderBaseConfiguration
{
};

class DecoderH264LibAVCodec : public Decoder,
                              public DecoderWithConfiguration<X264DecoderConfig>
{
private:
public:
    DecoderH264LibAVCodec();
    virtual ~DecoderH264LibAVCodec();
    virtual void open(const X264DecoderConfig &config) override;
    std::future<void> startProducing(const Source::ProducingConfiguration &config) override;
    void consume(const auvc::Frame &frame) override;
    void stopConsuming() override;
    std::future<void> stopProducing() override;
    std::future<void> close() override;
    std::map<uint16_t, std::vector<Source::Resolution>> getSupportedResolutions() const override;
    std::vector<auvc::FrameFormat> getSupportedFrameFormats() const override;
};