/*
 *  UVCCamera
 *  library and sample to access to UVC web camera on non-rooted Android device
 *
 * Copyright (c) 2014-2017 saki t_saki@serenegiant.com
 * Copyright (c) 2025 vshcryabets@gmail.com
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
 *  Files in the libjpeg-turbo, libusb, libuvc, rapidjson folder
 *  may have a different license, see the respective files.
 */
package com.jiangdg.uvc

enum class PixelFormat(val value: Int) {
    PIXEL_FORMAT_RAW(0),
    PIXEL_FORMAT_YUV(1),
    PIXEL_FORMAT_RGB565(2),
    PIXEL_FORMAT_RGBX(3),
    PIXEL_FORMAT_YUV420SP(4), // NV12
    PIXEL_FORMAT_NV21(5) // = YVU420SemiPlanar,NV21，但是保存到jpg颜色失真
}