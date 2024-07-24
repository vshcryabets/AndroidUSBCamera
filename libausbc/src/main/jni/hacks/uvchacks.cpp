/*
 * uvchacks
 * library and sample to access to UVC web camera on non-rooted Android device
 *
 * Copyright (c) 2024 vshcryabets@gmail.com
 *
 * File name: UVCPreview.h
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 * All files in the folder are under this Apache License, Version 2.0.
 * Files in the jni/libjpeg-turbo-1.50, libjpegturb0-2.1.1, jni/libusb folder may have a different license, see the respective files.
*/

#include "uvchacks.h"
#include "libuvc/libuvc_internal.h"
#include "utilbase.h"

/**
 * XXX add for non-rooted Android device, >= Android7
 * generate fake libusb_device according to specific params
 * and set it to uvc_device_t to access UVC device on Android7 or later
 */
uvc_error_t uvchack_get_device_with_fd(uvc_context_t *ctx, uvc_device_t **device,
                                       int vid, int pid, const char *serial, int fd, int busnum, int devaddr) {
    LOGD("call libusb_get_device_with_fd");
    ctx->own_usb_ctx = 1; // workaround for usb initialization in libuvc
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

libusb_device* uvchack_get_usb_device(uvc_device_t *device) {
    return device->usb_dev;
}
