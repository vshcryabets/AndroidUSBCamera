//
// Created by vshcryabets on 28.05.24.
//
#include "uvchacks.h"
#include "libuvc/libuvc_internal.h"
#include "utilbase.h"

/**
 * XXX add for non-rooted Android device, >= Android7
 * generate fake libusb_device according to specific params
 * and set it to uvc_device_t to access UVC device on Android7 or later
 */
uvc_error_t uvc_get_device_with_fd(uvc_context_t *ctx, uvc_device_t **device,
                                   int vid, int pid, const char *serial, int fd, int busnum, int devaddr) {
    LOGD("call libusb_get_device_with_fd");
    struct libusb_device *usb_dev = libusb_get_device_with_fd(ctx->usb_ctx, vid,
            pid, serial, fd, busnum, devaddr);

    if (LIKELY(usb_dev)) {
        *device = static_cast<uvc_device_t *>(malloc(sizeof(uvc_device_t/* *device */)));
        (*device)->ctx = ctx;
        (*device)->ref = 0;
        (*device)->usb_dev = usb_dev;
//		libusb_set_device_fd(usb_dev, fd);	// assign fd to libusb_device for non-rooted Android devices
        uvc_ref_device(*device);
        UVC_EXIT(UVC_SUCCESS);
        RETURN(UVC_SUCCESS, uvc_error_t);
    } else {
        LOGE("could not find specific device");
        *device = NULL;
        RETURN(UVC_ERROR_NO_DEVICE, uvc_error_t);
    }

}