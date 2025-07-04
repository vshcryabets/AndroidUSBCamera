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

object Constants {
    const val CTRL_SCANNING: Int = 0x00000001 // D0:  Scanning Mode
    const val CTRL_AE: Int = 0x00000002 // D1:  Auto-Exposure Mode
    const val CTRL_AE_PRIORITY: Int = 0x00000004 // D2:  Auto-Exposure Priority
    const val CTRL_AE_ABS: Int = 0x00000008 // D3:  Exposure Time (Absolute)
    const val CTRL_AR_REL: Int = 0x00000010 // D4:  Exposure Time (Relative)
    const val CTRL_FOCUS_ABS: Int = 0x00000020 // D5:  Focus (Absolute)
    const val CTRL_FOCUS_REL: Int = 0x00000040 // D6:  Focus (Relative)
    const val CTRL_IRIS_ABS: Int = 0x00000080 // D7:  Iris (Absolute)
    const val CTRL_IRIS_REL: Int = 0x00000100 // D8:  Iris (Relative)
    const val CTRL_ZOOM_ABS: Int = 0x00000200 // D9:  Zoom (Absolute)
    const val CTRL_ZOOM_REL: Int = 0x00000400 // D10: Zoom (Relative)
    const val CTRL_PANTILT_ABS: Int = 0x00000800 // D11: PanTilt (Absolute)
    const val CTRL_PANTILT_REL: Int = 0x00001000 // D12: PanTilt (Relative)
    const val CTRL_ROLL_ABS: Int = 0x00002000 // D13: Roll (Absolute)
    const val CTRL_ROLL_REL: Int = 0x00004000 // D14: Roll (Relative)
    const val CTRL_FOCUS_AUTO: Int = 0x00020000 // D17: Focus, Auto
    const val CTRL_PRIVACY: Int = 0x00040000 // D18: Privacy
    const val CTRL_FOCUS_SIMPLE: Int = 0x00080000 // D19: Focus, Simple
    const val CTRL_WINDOW: Int = 0x00100000 // D20: Window

    const val PU_BRIGHTNESS: Int = -0x7fffffff // D0: Brightness
    const val PU_CONTRAST: Int = -0x7ffffffe // D1: Contrast
    const val PU_HUE: Int = -0x7ffffffc // D2: Hue
    const val PU_SATURATION: Int = -0x7ffffff8 // D3: Saturation
    const val PU_SHARPNESS: Int = -0x7ffffff0 // D4: Sharpness
    const val PU_GAMMA: Int = -0x7fffffe0 // D5: Gamma
    const val PU_WB_TEMP: Int = -0x7fffffc0 // D6: White Balance Temperature
    const val PU_WB_COMPO: Int = -0x7fffff80 // D7: White Balance Component
    const val PU_BACKLIGHT: Int = -0x7fffff00 // D8: Backlight Compensation
    const val PU_GAIN: Int = -0x7ffffe00 // D9: Gain
    const val PU_POWER_LF: Int = -0x7ffffc00 // D10: Power Line Frequency
    const val PU_HUE_AUTO: Int = -0x7ffff800 // D11: Hue, Auto
    const val PU_WB_TEMP_AUTO: Int = -0x7ffff000 // D12: White Balance Temperature, Auto
    const val PU_WB_COMPO_AUTO: Int = -0x7fffe000 // D13: White Balance Component, Auto
    const val PU_DIGITAL_MULT: Int = -0x7fffc000 // D14: Digital Multiplier
    const val PU_DIGITAL_LIMIT: Int = -0x7fff8000 // D15: Digital Multiplier Limit
    const val PU_AVIDEO_STD: Int = -0x7fff0000 // D16: AnaXLogWrapper Video Standard
    const val PU_AVIDEO_LOCK: Int = -0x7ffe0000 // D17: AnaXLogWrapper Video Lock Status
    const val PU_CONTRAST_AUTO: Int = -0x7ffc0000 // D18: Contrast, Auto

    // uvc_status_class from libuvc.h
    const val STATUS_CLASS_CONTROL: Int = 0x10
    const val STATUS_CLASS_CONTROL_CAMERA: Int = 0x11
    const val STATUS_CLASS_CONTROL_PROCESSING: Int = 0x12

    // uvc_status_attribute from libuvc.h
    const val STATUS_ATTRIBUTE_VALUE_CHANGE: Int = 0x00
    const val STATUS_ATTRIBUTE_INFO_CHANGE: Int = 0x01
    const val STATUS_ATTRIBUTE_FAILURE_CHANGE: Int = 0x02
    const val STATUS_ATTRIBUTE_UNKNOWN: Int = 0xff
}