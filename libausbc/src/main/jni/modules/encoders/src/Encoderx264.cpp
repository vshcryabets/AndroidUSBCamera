#include "Encoderx264.h"
#include <iostream>
#include <string.h>

X264Encoder::X264Encoder()
{
}

X264Encoder::~X264Encoder()
{
}

void X264Encoder::open(const X264EncConfiguration &config)
{
    EncoderWithConfiguration<X264EncConfiguration>::open(config);
    x264_param_default_preset(&x264_param, "medium", "zerolatency");
    x264_param.i_width = config.width;
    x264_param.i_height = config.height;
    x264_param.i_fps_num = config.fps_num;
    x264_param.i_fps_den = config.fps_den;
    x264_param.i_keyint_max = config.keyframe_interval;
    x264_param.b_intra_refresh = config.intra_refresh;
    x264_param.b_annexb = config.annexb;
    x264_param.i_csp = X264_CSP_I420;
    x264_param.i_level_idc = config.level_idc; // H.264 Level ID
    x264_param.rc.i_rc_method = X264_RC_CRF;
    x264_param.rc.f_rf_constant = 23; // CRF value (lower is higher quality, typically 18-28)
    x264_param.rc.f_rf_constant_max = 25;
    x264_param_apply_profile(&x264_param, "high");

    encoder = x264_encoder_open(&x264_param);
    if (encoder == nullptr)
    {
        throw EncoderException(EncoderException::Type::NotInitialized, 
            "Failed to open x264 encoder with the provided configuration.");
    }
}

void X264Encoder::startProducing(const ProducingConfiguration &config)
{
    PushSource::startProducing(config);
    if (encoder == nullptr)
    {
        throw EncoderException(EncoderException::Type::NotInitialized, 
            "Encoder not initialized. Call open() first.");
    }
    frameYSize = x264_param.i_width * x264_param.i_height;
    x264_picture_alloc(&pic_in, x264_param.i_csp, x264_param.i_width, x264_param.i_height);
}

void X264Encoder::stopProducing() {
    x264_picture_clean(&pic_in);
    // x264_picture_clean(&pic_out);
}

void X264Encoder::close() {
    if (encoder != nullptr)
    {
        x264_encoder_close(encoder);
        encoder = nullptr;
    }
    PushSource::close();
}

void X264Encoder::stopConsuming() {
    // nothing to do
}

void X264Encoder::consume(const auvc::Frame &frame) {
    if (encoder == nullptr)
    {
        throw EncoderException(EncoderException::Type::NotInitialized, "Encoder not initialized. Call startProducing() first.");
    }
    x264_nal_t *nals = NULL;
    memcpy(pic_in.img.plane[0], frame.getData(), frameYSize);
    memcpy(pic_in.img.plane[1], frame.getData() + frameYSize, frameYSize / 4);
    memcpy(pic_in.img.plane[2], frame.getData() + frameYSize + frameYSize / 4, frameYSize / 4);
    int num_nals = 0;
    int bufferSize = x264_encoder_encode(encoder, &nals, &num_nals, &pic_in, &pic_out);
    if (bufferSize < 0)
    {
        throw EncoderException(EncoderException::Type::FailedToEncodeFrame, 
            "Failed to encode frame. Error code: " + 
            std::to_string(bufferSize));
    }
    
    if (bufferSize > 0 && nals != nullptr) {
        if (encodedBuffer.size() < bufferSize)
            encodedBuffer.resize(bufferSize);
        memcpy(encodedBuffer.data(), nals[0].p_payload, bufferSize);
        auvc::Frame encFrame = auvc::Frame(
            encoderConfig.width,
            encoderConfig.height,
            auvc::FrameFormat::ENCODED,
            encodedBuffer.data(),
            bufferSize,
            frame.getTimestamp()
        );
        if (encoderConfig.frameCallback) {
            encoderConfig.frameCallback(encFrame);
        }
        if (encoderConfig.consumer) {
            encoderConfig.consumer->consume(encFrame);
        }
    }
}

std::vector<auvc::FrameFormat> X264Encoder::getSupportedFrameFormats() const {
    return {};
}

std::map<uint16_t, std::vector<Source::Resolution>> X264Encoder::getSupportedResolutions() const {
    return {};
}
