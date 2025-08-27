#include "Encoderx264.h"
#include <iostream>

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

void X264Encoder::startProducing(const CaptureConfiguration &config)
{
    PushSource::startProducing(config);
    if (encoder == nullptr)
    {
        throw EncoderException(EncoderException::Type::NotInitialized, 
            "Encoder not initialized. Call open() first.");
    }
    x264_picture_alloc(&pic_in, x264_param.i_csp, x264_param.i_width, x264_param.i_height);
}

void X264Encoder::stopProducing() {
    x264_picture_clean(&pic_in);
    PushSource::stopProducing();
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

// x264_picture_t* X264Encoder::getPicIn() {
//     return &pic_in;
// }

// EncoderMultiBuffer X264Encoder::encodeFrame() {
//     if (encoder == nullptr)
//     {
//         throw EncoderException(EncoderException::Type::NotInitialized, "Encoder not initialized. Call open() first.");
//     }
//     EncoderMultiBuffer result;
//     x264_nal_t *nals = NULL;
//     int num_nals = 0;
//     result.totalSize = x264_encoder_encode(encoder, &nals, &num_nals, &pic_in, &pic_out);
//     if (result.totalSize < 0)
//     {
//         throw EncoderException(EncoderException::Type::FailedToEncodeFrame, 
//             "Failed to encode frame. Error code: " + 
//             std::to_string(result.totalSize));
//     }
//     for (int j = 0; j < num_nals; ++j) {
//         EncoderBuffer buffer;
//         buffer.data = nals[j].p_payload;
//         buffer.size = nals[j].i_payload;
//         result.buffers.push_back(buffer);
//     }
//     return result;
// }

// EncoderMultiBuffer X264Encoder::flush() {
//     EncoderMultiBuffer result;
//         // x264_nal_t *nals = NULL;
//     // int num_nals = 0;
//     // while (x264_encoder_encode(encoder, &nals, &num_nals, NULL, &pic_out) > 0)
//     // {
//     //     if (nals)
//     //     {
//     //         for (int j = 0; j < num_nals; ++j)
//     //         {
//     //             // output_file.write((char*)nals[j].p_payload, nals[j].i_payload);
//     //         }
//     //     }
//     // }
//     return result;
// }