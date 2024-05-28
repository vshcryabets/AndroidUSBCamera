//
// Created by vshcryabets on 28.05.24.
//

#ifndef ANDROIDUSBCAMERA_UVCHACKS_H
#define ANDROIDUSBCAMERA_UVCHACKS_H

#include "libuvc/libuvc.h"

uvc_error_t uvc_get_device_with_fd(uvc_context_t *ctx,
                                   uvc_device_t **device,
                                   int vid, int pid,
                                   const char *serial,
                                   int fd, int busnum, int devaddr);

#endif //ANDROIDUSBCAMERA_UVCHACKS_H
