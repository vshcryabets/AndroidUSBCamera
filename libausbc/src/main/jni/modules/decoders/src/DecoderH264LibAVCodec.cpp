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
    PushSource::open(config);
    auto codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!codec) {
        throw DecoderException(DecoderException::Type::DecoderNotFound, "Codec 'h264' not found");
    }
    codecContext = avcodec_alloc_context3(codec);
    if (!codecContext) {
        throw DecoderException(DecoderException::Type::NotInitialized, "Could not allocate video codec context");
    }
    int res = avcodec_open2(codecContext, codec, NULL);
    if (res < 0) {
        avcodec_free_context(&codecContext);
        throw DecoderException(DecoderException::Type::NotInitialized, "Could not open codec");
    }
}

std::future<void> DecoderH264LibAVCodec::close() {
    return std::async(std::launch::async, [this]() {
        if (codecContext)
            avcodec_free_context(&codecContext);
        PushSource::close().get();
        return;
    });
}

std::future<void> DecoderH264LibAVCodec::stopProducing() {
    return std::async(std::launch::async, [this]() {
        // No specific action needed for stopping producing in this implementation
    });
}

std::future<void> DecoderH264LibAVCodec::startProducing(
    const Source::ProducingConfiguration &config) {
    return PushSource::startProducing(config);
}

void DecoderH264LibAVCodec::consume(const auvc::Frame &frame) {
    AVPacket* packet = av_packet_alloc();

    if (!packet) {
        std::cout << "Failed to allocate AVPacket" << std::endl;
        return;
    }

    if (av_new_packet(packet, frame.getSize()) == 0) {
        memcpy(packet->data, frame.getData(), frame.getSize());
    } else {
        std::cout << "Failed to create new packet" << std::endl;
        av_packet_free(&packet);
        return;
    }

    int ret = avcodec_send_packet(codecContext, packet);
    
    if (ret < 0) {
        std::cout << "Error sending packet to decoder: " << ret << std::endl;
    } else {
        AVFrame* avframe = av_frame_alloc();
        while (ret >= 0) {
            ret = avcodec_receive_frame(codecContext, avframe);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) break;

            size_t frameSize = avframe->width * avframe->height * 3 / 2;
            auvc::OwnBufferFrame decodedFrame(
                avframe->width,
                avframe->height,
                auvc::FrameFormat::YUV420P,
                frameSize,
                frame.getTimestamp()
            );
            // copy data to decodedFrame
            // Y plane
            for (int i = 0; i < avframe->height; ++i) {
                memcpy(
                    decodedFrame.getData() + i * avframe->width,
                    avframe->data[0] + i * avframe->linesize[0],
                    avframe->width
                );
            }
            // U plane
            size_t uvOffset = avframe->width * avframe->height;
            for (int i = 0; i < avframe->height / 2; ++i) {
                memcpy(
                    decodedFrame.getData() + uvOffset + i * (avframe->width / 2),
                    avframe->data[1] + i * avframe->linesize[1],
                    avframe->width / 2
                );
            }
            // V plane
            size_t vOffset = uvOffset + (avframe->width / 2) * (avframe->height / 2);
            for (int i = 0; i < avframe->height / 2; ++i) {
                memcpy(
                    decodedFrame.getData() + vOffset + i * (avframe->width / 2),
                    avframe->data[2] + i * avframe->linesize[2],
                    avframe->width / 2
                );
            }
            try {
                pushFrame(decodedFrame);
            } catch (const std::exception &e) {
                std::cout << "Failed to push decoded frame: " << e.what() << std::endl;
            }
        }
        av_frame_free(&avframe);
    }
    av_packet_unref(packet);
    av_packet_free(&packet);
}

auvc::ExpectedResolutions DecoderH264LibAVCodec::getSupportedResolutions() const {
    return {};
}
std::vector<auvc::FrameFormat> DecoderH264LibAVCodec::getSupportedFrameFormats() const {
    return {auvc::FrameFormat::NONE};
}