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

import android.hardware.usb.UsbConstants
import android.hardware.usb.UsbManager
import com.jiangdg.usb.USBVendorId

interface LoadUsbDevices {
    data class UsbDevice(
        val usbDeviceId: Int,
        val displayName: String = "",
        val vendorName: String = "",
        val classesStr: String = ""
    )

    fun load(): List<UsbDevice>
}

class LoadUsbDevicesImpl(
    private val usbManager: UsbManager
) : LoadUsbDevices {
    override fun load(): List<LoadUsbDevices.UsbDevice> {
        val usbDevices = usbManager.deviceList

        val result = usbDevices.values.map { device ->
            val vendorName = USBVendorId.vendorName(device.vendorId)
            val vidPidStr = String.format("%04x:%04x", device.vendorId, device.productId)
            val classesList = mutableSetOf<Int>()
            classesList.add(device.deviceClass)
            if (device.deviceClass == UsbConstants.USB_CLASS_MISC) {
                for (i in 0 until device.interfaceCount) {
                    classesList.add(device.getInterface(i).interfaceClass)
                }
            }

            LoadUsbDevices.UsbDevice(
                usbDeviceId = device.deviceId,
                displayName = "$vidPidStr ${device.deviceName}",
                vendorName = if (vendorName.isEmpty()) "${device.vendorId}" else vendorName,
                classesStr = classesList.map {
                    USBVendorId.CLASSES[it] ?: "$it"
                }.joinToString(",\n")
            )
        }

        return result
    }

}