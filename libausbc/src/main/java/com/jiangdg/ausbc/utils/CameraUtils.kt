package com.jiangdg.ausbc.utils

import android.Manifest
import android.content.Context
import android.content.pm.PackageManager
import android.hardware.usb.UsbConstants
import android.hardware.usb.UsbDevice
import androidx.core.content.ContextCompat

/** Camera tools
 *
 * @author Created by jiangdg on 2022/7/19
 */
object CameraUtils {

    /**
     * Is camera contains mic
     *
     * @param device usb device
     * @return true contains
     */
    fun isCameraContainsMic(device: UsbDevice?): Boolean {
        device ?: return false
        var hasMic = false
        for (i in 0 until device.interfaceCount) {
            val cls = device.getInterface(i).interfaceClass
            if (cls == UsbConstants.USB_CLASS_AUDIO) {
                hasMic = true
                break
            }
        }
        return hasMic
    }

    fun hasCameraPermission(ctx: Context): Boolean{
        val locPermission = ContextCompat.checkSelfPermission(ctx, Manifest.permission.CAMERA)
        return locPermission == PackageManager.PERMISSION_GRANTED
    }
}