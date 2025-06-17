package com.jiangdg.ausbc.utils

import android.hardware.usb.UsbConstants
import android.hardware.usb.UsbDevice

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
}