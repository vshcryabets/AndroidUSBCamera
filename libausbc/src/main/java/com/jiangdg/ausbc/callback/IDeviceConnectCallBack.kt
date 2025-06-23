package com.jiangdg.ausbc.callback

import android.hardware.usb.UsbDevice
import com.jiangdg.usb.UsbControlBlock

/**
 * I device connect call back
 *
 * @author Created by jiangdg on 2022/7/19
 */
interface IDeviceConnectCallBack {

    /**
     * On detach dev
     *
     * @param device usb device
     */
    fun onDetachDec(device: UsbDevice?)

    /**
     * On connect dev
     *
     * @param device usb device
     */
    fun onConnectDev(device: UsbDevice?, ctrlBlock: UsbControlBlock? = null)

    /**
     * On dis connect dev
     *
     * @param device usb device
     */
    fun onDisConnectDec(device: UsbDevice?, ctrlBlock: UsbControlBlock? = null)

    /**
     * On cancel dev
     *
     * @param device usb device
     */
    fun onCancelDev(device: UsbDevice?)
}