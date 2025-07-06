#include <cstdint>
#include <iostream>
#include <vector>
#include <fstream>
#include "Encoderx264.h"

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
        EncoderMultiBuffer encoded = encoder.encodeFrame();

        if (encoded.totalSize > 0 && encoded.buffers.size() > 0)
        {
            std::cout << "Encoded frame " << i << ", size: " << encoded.totalSize << 
                " bytes, NALs: " << encoded.buffers.size() << std::endl;
            std::ofstream output_file("output" + std::to_string(i), std::ios::binary);
            for (int j = 0; j < encoded.buffers.size(); ++j)
            {
                output_file.write((char *)encoded.buffers[j].data, encoded.buffers[j].size);
            }
            output_file.close();
        }
    }

    // --- 7. Flush Encoder (Encode remaining delayed frames) ---
    std::cout << "Flushing encoder..." << std::endl;
    EncoderMultiBuffer encoded = encoder.flush();


    encoder.stop();
    encoder.close();
    std::cout << "Encoding complete. Output saved to output.h264" << std::endl;

    return 0;
}
