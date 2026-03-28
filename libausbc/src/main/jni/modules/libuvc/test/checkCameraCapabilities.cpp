#include <iostream>
#include <libusb.h>
#include <libuvc/libuvc.h>

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
        std::cout << "Device " << i << ": bus " << 
            (int)uvc_get_bus_number(dev) << 
            " addr " << (int)uvc_get_device_address(dev) 
            << std::endl;
    }

    uvc_free_device_list(list, 1);
    uvc_exit(ctx);
    return 0;
}