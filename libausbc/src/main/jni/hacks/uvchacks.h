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

#ifndef ANDROIDUSBCAMERA_UVCHACKS_H
#define ANDROIDUSBCAMERA_UVCHACKS_H

#include "libuvc/libuvc.h"
#include "libusb.h"

uvc_error_t uvchack_get_device_with_fd(uvc_context_t *ctx,
                                       uvc_device_t **device,
                                       int vid, int pid,
                                       const char *serial,
                                       int fd, int busnum, int devaddr);

libusb_device* uvchack_get_usb_device(uvc_device_t *device);

#endif //ANDROIDUSBCAMERA_UVCHACKS_H
