/*
 * Copyright 2025 vschryabets@gmail.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.vsh.uvc

import android.hardware.usb.UsbManager
import com.jiangdg.ausbc.utils.CheckCameraPermiussionUseCase

interface CheckRequirements {
    enum class Requirements {
        CAMERA_PERMISSION_REQUIRED,
        USB_DEVICE_PERMISSION_REQUIRED,
        NONE,
    }
    operator fun invoke(usbDeviceId: Int): Set<Requirements>
}

class CheckRequirementsImpl(
    private val checkCameraPermiussionUseCase: CheckCameraPermiussionUseCase,
    private val usbManager: UsbManager,
) : CheckRequirements {

    override fun invoke(usbDeviceId: Int): Set<CheckRequirements.Requirements> {
        val result = mutableSetOf<CheckRequirements.Requirements>()
        if (!checkCameraPermiussionUseCase()) {
            result.add(CheckRequirements.Requirements.CAMERA_PERMISSION_REQUIRED)
        }
        val usbDevice = usbManager.deviceList.values.firstOrNull {
            it.deviceId == usbDeviceId
        } ?: return result
        if (!usbManager.hasPermission(usbDevice)) {
            result.add(CheckRequirements.Requirements.USB_DEVICE_PERMISSION_REQUIRED)
        }
        return result
    }
}