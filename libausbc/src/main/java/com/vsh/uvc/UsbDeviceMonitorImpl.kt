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

import android.content.BroadcastReceiver
import android.content.Context
import android.content.ContextWrapper
import android.content.Intent
import android.content.IntentFilter
import android.hardware.usb.UsbConstants
import android.hardware.usb.UsbDevice
import android.hardware.usb.UsbManager
import com.jiangdg.usb.USBVendorId
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.update
import java.util.concurrent.atomic.AtomicInteger

class UsbDeviceMonitorImpl(
    private val usbManager: UsbManager,
    private val contextWrapper: ContextWrapper,
): UsbDevicesMonitor {
    private val sessionCounter = AtomicInteger()
    private val sessions = HashSet<Int>()
    private val sessionsLock = Any()
    private val usbDevices = MutableStateFlow<List<UsbDevicesMonitor.UsbDevice>>(emptyList())
    private val detached = MutableStateFlow<List<UsbDevicesMonitor.UsbDevice>>(emptyList())
    private val attached = MutableStateFlow<List<UsbDevicesMonitor.UsbDevice>>(emptyList())

    private val usbReceiver = object : BroadcastReceiver() {
        override fun onReceive(context: Context, intent: Intent) {
            val action = intent.action
            if (action == UsbManager.ACTION_USB_DEVICE_ATTACHED) {
                val device = intent.getParcelableExtra<UsbDevice>(UsbManager.EXTRA_DEVICE)
                if (device != null) {
                    val mappedValue = map(device)
                    attached.update { listOf(mappedValue) }
                    usbDevices.update {
                        it + mappedValue
                    }
                }
            } else if (action == UsbManager.ACTION_USB_DEVICE_DETACHED) {
                val device = intent.getParcelableExtra<UsbDevice>(UsbManager.EXTRA_DEVICE)
                if (device != null) {
                    val usbDeviceId = device.deviceId
                    usbDevices.update { currentDevices ->
                        currentDevices.firstOrNull { it.usbDeviceId == usbDeviceId }?.let { old ->
                            detached.update { listOf(old) }
                        }
                        currentDevices.filter { it.usbDeviceId != usbDeviceId }
                    }
                }
            }
        }
    }

    override fun getUsbDevices(): List<UsbDevicesMonitor.UsbDevice> = usbDevices.value
    override fun usbDevices(): Flow<List<UsbDevicesMonitor.UsbDevice>> = usbDevices
    override fun attached(): Flow<List<UsbDevicesMonitor.UsbDevice>> = detached
    override fun detached(): Flow<List<UsbDevicesMonitor.UsbDevice>> = attached

    override fun startSession(): Int {
        val id = sessionCounter.incrementAndGet()
        synchronized(sessionsLock) {
            if (sessions.isEmpty()) {
                internalStart()
            }
            sessions.add(id)
        }
        return id
    }

    private fun map(androidUsbDevice: UsbDevice): UsbDevicesMonitor.UsbDevice {
        val vendorName = USBVendorId.vendorName(androidUsbDevice.vendorId)
        val vidPidStr = String.format("%04x:%04x", androidUsbDevice.vendorId, androidUsbDevice.productId)
        val classesList = mutableSetOf<Int>()
        classesList.add(androidUsbDevice.deviceClass)
        if (androidUsbDevice.deviceClass == UsbConstants.USB_CLASS_MISC) {
            for (i in 0 until androidUsbDevice.interfaceCount) {
                classesList.add(androidUsbDevice.getInterface(i).interfaceClass)
            }
        }

        return UsbDevicesMonitor.UsbDevice(
            usbDeviceId = androidUsbDevice.deviceId,
            displayName = "$vidPidStr ${androidUsbDevice.deviceName}",
            vendorName = if (vendorName.isEmpty()) "${androidUsbDevice.vendorId}" else vendorName,
            classesStr = classesList.map {
                USBVendorId.CLASSES[it] ?: "$it"
            }.joinToString(",\n")
        )
    }

    private fun internalStart() {
        forceReload()
        val filter = IntentFilter(UsbManager.ACTION_USB_DEVICE_ATTACHED)
        filter.addAction(UsbManager.ACTION_USB_DEVICE_DETACHED)
        contextWrapper.registerReceiver(usbReceiver, filter, Context.RECEIVER_EXPORTED)
    }

    private fun internalStop() {
        contextWrapper.unregisterReceiver(usbReceiver)
        usbDevices.update { emptyList() }
    }

    override fun stopSesstion(sessionId: Int) {
        synchronized(sessionsLock) {
            sessions.remove(sessionId)
            if (sessions.isEmpty()) {
                internalStop()
            }
        }
    }

    override fun forceReload() {
        val usbDevices = usbManager.deviceList
        val result = usbDevices.values.map { device -> map(device) }
        this.usbDevices.update { result }
    }
}