/*
 *  UVCCamera
 *  library and sample to access to UVC web camera on non-rooted Android device
 *
 * Copyright (c) 2014-2017 saki t_saki@serenegiant.com
 * Copyright (c) 2025 vschryabets@gmail.com
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 *  All files in the folder are under this Apache License, Version 2.0.
 *  Files in the libjpeg-turbo, libusb, libuvc folder
 *  may have a different license, see the respective files.
 */
package com.jiangdg.uvc

import com.jiangdg.usb.UsbControlBlock

class UVCCamera2 : IUvcCamera {
    private var nativePtr : Long = 0L;

    constructor() {
        nativePtr = nativeCreate()
    }

    override fun open(usbControlBlock: UsbControlBlock) {
    }

    override fun close() {
        if (nativePtr != 0L) {
            nativeClose(nativePtr)
        }
    }

    override fun stopCapturing() {
        if (nativePtr != 0L) {
            nativeStopCapturing(nativePtr)
        }
    }

    private external fun nativeCreate(): Long
    private external fun nativeRelease(ptr: Long)
    private external fun nativeStopCapturing(ptr: Long)
    private external fun nativeClose(ptr: Long)

    companion object {
        init {
            System.loadLibrary("native")
        }
    }
}
