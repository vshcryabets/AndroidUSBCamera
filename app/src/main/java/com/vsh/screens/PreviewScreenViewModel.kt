package com.vsh.screens

import androidx.lifecycle.ViewModel
import androidx.lifecycle.ViewModelProvider
import com.vsh.uvc.UsbDevicesMonitor

class PreviewScreenViewModelFactory(
    private val usbDevicesMonitor: UsbDevicesMonitor,
) : ViewModelProvider.Factory {
    @Suppress("UNCHECKED_CAST")
    override fun <T : ViewModel> create(modelClass: Class<T>): T =
        PreviewScreenViewModel(
            usbDevicesMonitor = usbDevicesMonitor,
        ) as T
}

class PreviewScreenViewModel(
    private val usbDevicesMonitor: UsbDevicesMonitor,
) {
}