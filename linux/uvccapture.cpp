#include <iostream>
#include <string.h>
#include <unistd.h>

#include "Source.h"
#include "UvcCamera.h"
#include "TestSource.h"

// SDL
#include "SdlTools.h"
#include "ImageUseCases.h"

static void errno_exit(const char *s)
{
    fprintf(stderr, "%s error %d, %s\n", s, errno, strerror(errno));
    exit(EXIT_FAILURE);
}

int main(void)
{
    const char *videodevice = "/dev/video0";

    UvcCamera::Configuration deviceConfig = {
        .dev_name = videodevice,
    };

    UvcCamera uvcCamera;
    try
    {
        uvcCamera.open(deviceConfig);

        Source::CaptureConfigutation captureConfig = {
            .width = 640,
            .height = 480,
            .fps = 30.0f};
        // TestSource testSource;
        Source &camera = uvcCamera;
        SdlWindow wnd("UVC preview", captureConfig.width, captureConfig.height);
        SdlLoop loop({.needDelay = false});
        SDL_Texture *texture = SDL_CreateTexture(wnd.getRenderer(),
                                                 SDL_PIXELFORMAT_RGBA32,
                                                 SDL_TEXTUREACCESS_STREAMING,
                                                 captureConfig.width, captureConfig.height);

        int frame_number = 0;
        camera.startCapturing(captureConfig);
        ConvertYUV422toRGBAUseCase convertUseCase;
        ConvertBitmapUseCase::Buffer dst = {
            .buffer = new uint8_t[captureConfig.width * captureConfig.height * 4],
            .capacity = captureConfig.width *captureConfig.height * 4,
            .size = 0,
            .width = 0,
            .height = 0};

        loop.setDrawCallback(
            [&camera, &frame_number, &texture, &wnd, &dst, &convertUseCase, &captureConfig]
            (uint32_t frameCounter)
        {
            if (camera.waitNextFrame()) {
                Source::Frame result = camera.readFrame();
                if (result.data != nullptr) {
                    frame_number++;

                    convertUseCase.convert(dst,
                        {
                            .buffer = result.data,
                            .capacity = result.size,
                            .size = result.size,
                            .width = captureConfig.width,
                            .height = captureConfig.height
                        }
                    );
                    // char filename[15];
                    // sprintf(filename, "frame-%d.bin", frame_number);
                    // FILE *fp=fopen(filename,"wb");
                    // fwrite(result.data, result.size, 1, fp);
                    // fflush(fp);
                    // fclose(fp);
                    SDL_UpdateTexture(texture, NULL, dst.buffer, captureConfig.width * 4);
                    wnd.clear();
                    SDL_RenderCopy(wnd.getRenderer(), texture, NULL, NULL);
                    SDL_RenderPresent(wnd.getRenderer());
                };
            } else {
                if (EINTR != errno)
                    errno_exit("select");
            } 
        });
        loop.loop();
        delete[] dst.buffer;
        camera.stopCapturing();
        camera.close();
    }
    catch (UvcException &e)
    {
        std::cerr << "Error opening camera: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}
