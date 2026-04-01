#include <iostream>
#include <chrono>
#include <thread>

#include <libusb.h>
#include <libuvc/libuvc.h>
#include <libuvc/libuvc_internal.h> // For internal structures and definitions

#include "utilbase_cpp.h" // For convertToHexStringWithOffsets

class UvcFramesReader {
    private:
        uvc_device_handle_t* deviceHandle;

        static void cb(uvc_frame_t *frame, void *ptr) {
            std::cout << "Received a frame callback!" << std::endl;
            std::cout << "Frame details: " << std::endl;
            std::cout << "  Width: " << frame->width << std::endl;
            std::cout << "  Height: " << frame->height << std::endl;
            std::cout << "  Size: " << frame->data_bytes << std::endl;
            std::cout << "  Sequence: " << frame->sequence << std::endl;
            std::cout << "  Format: " << frame->frame_format << std::endl << std::endl;
            const uint8_t* bytes = static_cast<const uint8_t*>(frame->data);


            const size_t count = frame->data_bytes < 32 ? frame->data_bytes : 32;
            std::string hexString = auvc::convertToHexStringWithOffsets(bytes, count);
            std::cout << "First " << count << " bytes (hex with offsets):" << std::endl;
            std::cout << hexString << std::endl;
        }
    public:
        UvcFramesReader(uvc_device* dev) : deviceHandle(nullptr) {
            uvc_open(dev, &deviceHandle);
        }

        ~UvcFramesReader() {
            uvc_close(deviceHandle);
        }

        void readFrames() {
            std::cout << "Reading frames from the device..." << std::endl;
            uvc_stream_ctrl_t ctrl;
            uvc_error_t res;

            res = uvc_get_stream_ctrl_format_size(
                deviceHandle, 
                &ctrl, 
                UVC_FRAME_FORMAT_YUYV,  /* UVC_FRAME_FORMAT_MJPEG */
                128, 96, 10
            );

            if (res < 0) {
                uvc_perror(res, "get_mode");
                return;
            }

            res = uvc_start_streaming(deviceHandle, &ctrl, UvcFramesReader::cb, this, 0);

            if (res < 0) {
                uvc_perror(res, "start_streaming");
            } else {
                std::cout << "Start streaming..." << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(3));
                uvc_stop_streaming(deviceHandle);
            }
        }
};

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
    size_t maxDevices = 0;

    for (int i = 0; list[i] != NULL; i++) {
        uvc_device_t *dev = list[i];
        uvc_device_descriptor_t* desc;
        uvc_get_device_descriptor(dev, &desc);
        std::cout << (i+1) << ". Device " << i << ": bus " << 
            (int)uvc_get_bus_number(dev) << 
            " addr " << (int)uvc_get_device_address(dev) <<
            " " << std::hex << desc->idVendor << ":" <<
            desc->idProduct << std::dec <<
            " name: " << (desc->product ? desc->product : "Unknown") <<
            std::endl;
        maxDevices++;
    }
    std::cout << "0. Exit" << std::endl;

    int selectedDevice = -1;
    std::cout << "Select a device: ";
    std::cin >> selectedDevice;
    if (selectedDevice > 0 && selectedDevice <= maxDevices) {
        UvcFramesReader reader(list[selectedDevice - 1]);
        reader.readFrames();
    }

    uvc_free_device_list(list, 1);
    uvc_exit(ctx);
    return 0;
}