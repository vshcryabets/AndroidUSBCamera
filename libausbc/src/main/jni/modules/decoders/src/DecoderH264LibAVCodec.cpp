#include "DecoderH264LibAVCodec.h"
#include <iostream>
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

void DecoderH264LibAVCodec::start() {
}

void DecoderH264LibAVCodec::stop() {
}

void DecoderH264LibAVCodec::close() {
}

DecoderMultiBuffer DecoderH264LibAVCodec::decodeFrame(const DecoderBuffer &inputBuffer) {
    return DecoderMultiBuffer();
}