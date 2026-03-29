#include <iostream>
#include <libusb.h>
#include <libuvc/libuvc.h>
#include <libuvc/libuvc_internal.h> // For internal structures and definitions

int main() {
    uvc_context_t *ctx;
    uvc_device_t **list;
    uvc_error_t res;

    res = uvc_init(&ctx, NULL);
    if (res < 0) {
        std::cerr << "uvc_init error: " << uvc_strerror(res) << std::endl;
        return res;
    }

    res = uvc_get_device_list(ctx, &list);
    if (res < 0) {
        std::cerr << "uvc_get_device_list error: " << uvc_strerror(res) << std::endl;
        uvc_exit(ctx);
        return res;
    }

    std::cout << std::endl;

    for (int i = 0; list[i] != NULL; i++) {
        uvc_device_t *dev = list[i];
        std::cout << "\tDevice " << i << ": bus " << 
            (int)uvc_get_bus_number(dev) << 
            " addr " << (int)uvc_get_device_address(dev) 
            << std::endl;
        uvc_device_handle_t *devh;
        uvc_stream_ctrl_t ctrl;

        res = uvc_open(dev, &devh);
        if (res < 0) {
            std::cerr << "uvc_open error: " << uvc_strerror(res) << std::endl;
            continue;
        }

        if (devh->info->stream_ifs) {
            int stream_idx = 0;
            for (auto *stream_if = devh->info->stream_ifs;
                stream_if;
                stream_if = stream_if->next) {
                ++stream_idx;
                for (auto *fmt_desc = stream_if->format_descs;
                    fmt_desc;
                    fmt_desc = fmt_desc->next) {
                    uvc_vs_desc_subtype subtype = fmt_desc->bDescriptorSubtype;
                    std::cout << " Subtype: " << (int)subtype << " ";
                    switch (subtype)
                    {
                    case UVC_VS_FORMAT_UNCOMPRESSED:
                        std::cout << "UVC_VS_FORMAT_UNCOMPRESSED";
                        break;
                    case UVC_VS_FORMAT_MJPEG:
                        std::cout << "UVC_VS_FORMAT_MJPEG";
                        break;
                    default:
                        std::cout << "Unsupported format subtype";
                        continue; // Skip unsupported formats
                    }
                    std::cout << std::endl;
                    std::cout << " Frame desc pointer = " << fmt_desc->frame_descs << std::endl;
                    for (const auto *frame_desc = fmt_desc->frame_descs;
                        frame_desc;
                        frame_desc = frame_desc->next) {
                        std::cout << "\tStream " << stream_idx << ": format " << (int)fmt_desc->bFormatIndex
                            << ", resolution " << frame_desc->wWidth << "x" << frame_desc->wHeight
                            << " Intervals: ";
                        if (frame_desc->bFrameIntervalType == 1) {
                            // discrete intervals
                            for (auto interval = frame_desc->intervals; *interval; ++interval) {
                                float fps = 10000000.0f / (float)*interval;
                                std::cout << fps << " fps, ";
                            }
                        } else {
                            std::cout << "Continious intevals " << frame_desc->dwMinFrameInterval << " to " << frame_desc->dwMaxFrameInterval
                                << " with step " << frame_desc->dwFrameIntervalStep;
                        }
                        std::cout << std::endl;
                    }
                }
            }
        }
        uvc_close(devh);
    }

    uvc_free_device_list(list, 1);
    uvc_exit(ctx);
    return 0;
}