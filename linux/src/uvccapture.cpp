#include <iostream>
#include <string.h>
#include <unistd.h>

#include "UvcCamera.h"

// SDL
#include "SdlTools.h"
#include "ImageUseCases.h"

static void errno_exit(const char *s) {
    fprintf(stderr, "%s error %d, %s\n", s, errno, strerror(errno));
    exit(EXIT_FAILURE);
}

int main(void) {
    const char *videodevice = "/dev/video0";

    UvcCamera camera;
    camera.open_device(videodevice);
    camera.init_device(videodevice);
    camera.start_capturing();

    SdlWindow wnd("UVC preview", 640, 480);
    SdlLoop loop(
        {
            .needDelay = false
        }
    );
    SDL_Texture* texture = SDL_CreateTexture(wnd.getRenderer(),
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        camera.width, camera.height);

    int frame_number = 0;
    ConvertYUV422toRGBAUseCase convertUseCase;
    ConvertBitmapUseCase::Buffer dst = {
        .buffer = new uint8_t[camera.width * camera.height * 4],
        .capacity = camera.width * camera.height * 4,
        .size = 0,
        .width = 0,
        .height = 0
    };

    loop.setDrawCallback([&camera, &frame_number, &texture, &wnd, &dst, &convertUseCase](uint32_t frameCounter) {
        fd_set fds;
        struct timeval tv;
        int r;

        FD_ZERO(&fds);
        FD_SET(camera.getFd(), &fds);

        /* Timeout. */
        tv.tv_sec = 2;
        tv.tv_usec = 0;

        r = select(camera.getFd() + 1, &fds, NULL, NULL, &tv);

        if (0 == r) {
            fprintf(stderr, "select timeout\n");
            exit(EXIT_FAILURE);
        }
        if (r > 0 ) {
            UvcCamera::Frame result = camera.readFrame();
            if (result.data != nullptr) {
                frame_number++;
                convertUseCase.convert(dst,
                    {
                        .buffer = result.data,
                        .capacity = result.size,
                        .size = result.size,
                        .width = camera.width,
                        .height = camera.height
                    }
                );
                // char filename[15];
                // sprintf(filename, "frame-%d.bin", frame_number);
                // FILE *fp=fopen(filename,"wb");
                // fwrite(result.data, result.size, 1, fp);
                // fflush(fp);
                // fclose(fp);
                SDL_UpdateTexture(texture, NULL, dst.buffer, camera.width * 4);
                wnd.clear();
                SDL_RenderCopy(wnd.getRenderer(), texture, NULL, NULL);
                SDL_RenderPresent(wnd.getRenderer());
            };
        } else  if (-1 == r) {
            if (EINTR != errno)
                errno_exit("select");
        }
    });
    loop.loop();

    delete[] dst.buffer;

    camera.stop_capturing();

    camera.uninit_device();
    camera.close_device();

    return 0;
}
