#include <iostream>
#include <libusb.h>
#include <vector>

int main() {
    libusb_context *ctx = nullptr;
    libusb_device **devs;
    ssize_t cnt;

    int r = libusb_init(&ctx);
    if (r < 0) {
        std::cerr << "Init Error " << r << std::endl;
        return 1;
    }

    //libusb_set_option(ctx, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_INFO);

    cnt = libusb_get_device_list(ctx, &devs);
    if (cnt < 0) {
        std::cerr << "Get Device List Error" << std::endl;
        return 1;
    }

    std::cout << "Found devices: " << cnt << std::endl;
    std::cout << "------------------------------------" << std::endl;

    for (ssize_t i = 0; i < cnt; i++) {
        libusb_device_descriptor desc;
        r = libusb_get_device_descriptor(devs[i], &desc);
        if (r < 0) continue;

        printf("Device %02zd: ID %04x:%04x\n", i, desc.idVendor, desc.idProduct);
    }

    // 3. Cleanup resources
    libusb_free_device_list(devs, 1);
    libusb_exit(ctx);

    return 0;
}