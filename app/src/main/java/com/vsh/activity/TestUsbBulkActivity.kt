package com.vsh.activity

import android.app.PendingIntent
import android.content.Context
import android.content.Intent
import android.hardware.usb.UsbDevice
import android.hardware.usb.UsbDeviceConnection
import android.hardware.usb.UsbEndpoint
import android.hardware.usb.UsbManager
import android.os.Bundle
import androidx.activity.ComponentActivity
import java.nio.charset.Charset

class UsbHelper(private val context: Context) {
    private val usbManager = context.getSystemService(Context.USB_SERVICE) as UsbManager
    private val ACTION_USB_PERMISSION = "com.example.USB_PERMISSION"

    fun findAndConnectDevice(): Pair<UsbDeviceConnection, UsbDevice>? {
        val deviceList = usbManager.deviceList
        val device = deviceList.values.find { it.vendorId == 0x0000 && it.productId == 0x0001 }

        if (device == null) {
            println("Device not found")
            return null
        }

        val permissionIntent = PendingIntent.getBroadcast(
            context, 0, Intent(ACTION_USB_PERMISSION), PendingIntent.FLAG_IMMUTABLE
        )

        usbManager.requestPermission(device, permissionIntent)

        if (!usbManager.hasPermission(device)) {
            println("No permission to access USB device")
            return null
        }

        val usbDeviceConnection = usbManager.openDevice(device) ?: return null
        val interfaceUsb = device.getInterface(0)

        val endpointOut = interfaceUsb.getEndpoint(0) // OUT
        val endpointIn = interfaceUsb.getEndpoint(1)  // IN

        usbDeviceConnection.claimInterface(interfaceUsb, true)

        println("Device connected: ${device.deviceName}")
        return Pair(usbDeviceConnection, device)
    }

    fun sendData(connection: UsbDeviceConnection, endpointOut: UsbEndpoint, data: String) {
        val bytes = data.toByteArray(Charset.defaultCharset())
        val result = connection.bulkTransfer(endpointOut, bytes, bytes.size, 1000)
        if (result > 0) {
            println("Sent: $data")
        } else {
            println("Failed to send data")
        }
    }

    fun receiveData(connection: UsbDeviceConnection, endpointIn: UsbEndpoint, bufferSize: Int = 64): String {
        val buffer = ByteArray(bufferSize)
        val received = connection.bulkTransfer(endpointIn, buffer, buffer.size, 1000)
        return if (received > 0) {
            String(buffer, 0, received, Charset.defaultCharset())
        } else {
            "No data received"
        }
    }
}

class TestUsbBulkActivity: ComponentActivity() {
    private lateinit var usbHelper: UsbHelper

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        usbHelper = UsbHelper(this)

        val pair = usbHelper.findAndConnectDevice()
        if (pair != null) {
            val device = pair.second
            val connection = pair.first
            val interfaceUsb = device.getInterface(0)
            val endpointOut = interfaceUsb.getEndpoint(0)
            val endpointIn = interfaceUsb.getEndpoint(1)

            usbHelper.sendData(connection, endpointOut, "Hello USB!")
            val response = usbHelper.receiveData(connection, endpointIn)
            println("Received: $response")
        }
    }
}