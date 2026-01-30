#include "DecoderH264LibAVCodec.h"
#include <iostream>
#include <future>

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavutil/frame.h>
    #include <libavutil/mem.h>
}

DecoderH264LibAVCodec::~DecoderH264LibAVCodec() {
}

DecoderH264LibAVCodec::DecoderH264LibAVCodec() {
}

void DecoderH264LibAVCodec::open(const X264DecoderConfig &config) {
    auto codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!codec) {
        throw DecoderException(DecoderException::Type::DecoderNotFound, "Codec 'h264' not found");
    }
    auto c = avcodec_alloc_context3(codec);
    if (!c) {
        throw DecoderException(DecoderException::Type::NotInitialized, "Could not allocate video codec context");
    }
}

std::future<void> DecoderH264LibAVCodec::close() {
    return std::async(std::launch::async, [this]() {
        PushSource::close().get();
        return;
    });
}

void DecoderH264LibAVCodec::stopConsuming() {
    // Implementation goes here
}

std::future<void> DecoderH264LibAVCodec::stopProducing() {
    return std::async(std::launch::async, [this]() {
    });
}

std::future<void> DecoderH264LibAVCodec::startProducing(
    const Source::ProducingConfiguration &config) {
    return PushSource::startProducing(config);
}

void DecoderH264LibAVCodec::consume(const auvc::Frame &frame) {
    // Implementation goes here
    std::cout << "DecoderH264LibAVCodec::consume called with frame of size: " << frame.getSize() << std::endl;
}

auvc::ExpectedResolutions DecoderH264LibAVCodec::getSupportedResolutions() const {
    return {};
}
std::vector<auvc::FrameFormat> DecoderH264LibAVCodec::getSupportedFrameFormats() const {
    return {auvc::FrameFormat::NONE};
}