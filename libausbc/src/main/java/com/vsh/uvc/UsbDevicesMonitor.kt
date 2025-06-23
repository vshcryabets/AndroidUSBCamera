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

import kotlinx.coroutines.flow.Flow

interface UsbDevicesMonitor {
    data class UsbDevice(
        val usbDeviceId: Int,
        val displayName: String = "",
        val vendorName: String = "",
        val classesStr: String = "",
        val timestamp: Long = System.currentTimeMillis()
    )

    fun getUsbDevices(): List<UsbDevice>
    fun usbDevices(): Flow<List<UsbDevice>>
    fun attached(): Flow<List<UsbDevice>>
    fun detached(): Flow<List<UsbDevice>>
    fun startSession(): Int
    fun stopSesstion(sessionId: Int)
    fun forceReload()
}