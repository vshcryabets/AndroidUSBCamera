#include <cstdint>
#include <x264.h>
#include <iostream>
#include <vector>
#include <fstream>

// For basic error handling
#define CHECK_X264(ret, msg)                                                         \
    if (ret < 0)                                                                     \
    {                                                                                \
        std::cerr << "x264 error: " << msg << " (Code: " << ret << ")" << std::endl; \
        return 1;                                                                    \
    }

class Encoder
{
public:
    Encoder() = default;
    virtual ~Encoder() {};
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void encodeFrame() = 0;
};

struct EncoderBaseConfiguration
{
    uint32_t width;
    uint32_t height;
    uint16_t fps_num;
    uint16_t fps_den;
};

template <typename T>
class EncoderWithConfiguration
{
private:
    T config;

public:
    EncoderWithConfiguration() = default;
    virtual void open(const T &config)
    {
        this->config = config;
    }
    virtual const T &getConfiguration() const
    {
        return config;
    }
    virtual void close() = 0;
    virtual ~EncoderWithConfiguration() {}
};

struct X264Configuration : public EncoderBaseConfiguration
{
    uint16_t keyframe_interval; // Keyframe interval in seconds
    uint8_t level_idc;          // H.264 Level ID
    uint8_t profile;            // H.264 Profile (e.g., baseline, main, high)
    bool annexb;                // Use Annex B format for NAL units
    bool intra_refresh;         // Use IDR refresh
    float crf;                  // Constant Rate Factor (CRF) value for quality control
};

class X264Encoder : public Encoder, public EncoderWithConfiguration<X264Configuration>
{
private:
    x264_param_t x264_param;
    x264_t *encoder = nullptr;
    x264_picture_t pic_in;
    x264_picture_t pic_out;
    x264_nal_t *nals = NULL;
    int num_nals = 0;

public:
    X264Encoder()
    {
    }
    virtual ~X264Encoder()
    {
    }
    virtual void open(const X264Configuration &config) override
    {
        EncoderWithConfiguration<X264Configuration>::open(config);
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
            std::cerr << "Failed to open x264 encoder with the provided configuration." << std::endl;
            // TODO throw an exception or handle the error appropriately
            return;
        }
    }

    void start() override
    {
        if (encoder == nullptr)
        {
            std::cerr << "Encoder not initialized. Call open() first." << std::endl;
            return;
        }
        x264_picture_alloc(&pic_in, x264_param.i_csp, x264_param.i_width, x264_param.i_height);
    }

    void stop() override {
        x264_picture_clean(&pic_in);
        x264_picture_clean(&pic_out);

    }

    void close() override {
        stop();
        if (encoder != nullptr)
        {
            x264_encoder_close(encoder);
            encoder = nullptr;
        }
    }

    x264_picture_t* getPicIn() {
        return &pic_in;
    }

    void encodeFrame() override {
        if (encoder == nullptr)
        {
            std::cerr << "Encoder not initialized. Call open() first." << std::endl;
            return;
        }
        int frame_size = x264_encoder_encode(encoder, &nals, &num_nals, &pic_in, &pic_out);
        if (frame_size < 0)
        {
            std::cerr << "Failed to encode frame. Error code: " << frame_size << std::endl;
            return;
        }
    }
};

int main()
{
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

    // --- 6. Encode Frames ---
    const int total_frames = 100; // Number of frames to encode
    x264_picture_t *pic_in = encoder.getPicIn();
    for (int i = 0; i < total_frames; ++i)
    {
        // Simulate a YUV420P frame (replace with your actual frame data)
        // Y plane: width * height bytes
        // U plane: (width/2) * (height/2) bytes
        // V plane: (width/2) * (height/2) bytes

        // For demonstration, we'll fill with dummy data that changes per frame
        // This will result in a moving grayscale pattern with varying chroma
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
        encoder.encodeFrame();

        if (frame_size > 0 && nals)
        {
            std::cout << "Encoded frame " << i << ", size: " << frame_size << " bytes, NALs: " << num_nals << std::endl;
            std::ofstream output_file("output" + std::to_string(i), std::ios::binary);
            for (int j = 0; j < num_nals; ++j)
            {
                output_file.write((char *)nals[j].p_payload, nals[j].i_payload);
            }
            output_file.close();
        }
    }

    // --- 7. Flush Encoder (Encode remaining delayed frames) ---
    std::cout << "Flushing encoder..." << std::endl;
    x264_nal_t *nals = NULL;
    int num_nals = 0;
    while (x264_encoder_encode(encoder, &nals, &num_nals, NULL, &pic_out) > 0)
    {
        if (nals)
        {
            for (int j = 0; j < num_nals; ++j)
            {
                // output_file.write((char*)nals[j].p_payload, nals[j].i_payload);
            }
        }
    }

    // --- 8. Cleanup ---
    // output_file.close();
    x264_picture_clean(&pic_in);
    x264_encoder_close(encoder);
    std::cout << "Encoding complete. Output saved to output.h264" << std::endl;

    return 0;
}
