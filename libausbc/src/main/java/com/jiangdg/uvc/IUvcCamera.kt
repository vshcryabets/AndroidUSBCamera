package com.jiangdg.uvc

import com.jiangdg.usb.UsbControlBlock

interface IUvcCamera {
    fun open(usbControlBlock: UsbControlBlock)
    fun close()
}