package com.vsh.uvc

import android.content.Context
import android.hardware.usb.UsbManager
import androidx.core.content.ContextCompat

interface CheckRequirements {
    enum class Requirements {
        CAMERA_PERMISSION_REQUIRED,
        USB_DEVICE_PERMISSION_REQUIRED,
        NONE,
    }
    operator fun invoke(usbDeviceId: Int): Set<Requirements>
}

class CheckRequirementsImpl(
    private val appContext: Context,
    private val usbManager: UsbManager,
) : CheckRequirements {

    override fun invoke(usbDeviceId: Int): Set<CheckRequirements.Requirements> {
        val result = mutableSetOf<CheckRequirements.Requirements>()
        if (ContextCompat.checkSelfPermission(appContext, android.Manifest.permission.CAMERA) !=
            android.content.pm.PackageManager.PERMISSION_GRANTED) {
            result.add(CheckRequirements.Requirements.CAMERA_PERMISSION_REQUIRED)
        }
        return result
    }
}