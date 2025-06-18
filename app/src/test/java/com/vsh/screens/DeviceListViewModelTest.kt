package com.vsh.screens

import com.vsh.uvc.CheckRequirements
import com.vsh.uvc.JpegBenchmark
import com.vsh.uvc.UsbDevicesMonitor
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.SharedFlow
import org.junit.Assert
import org.junit.Test

class DeviceListViewModelTest {
    val checkRequirementsAllGranted = object: CheckRequirements {
        override fun invoke(usbDeviceId: Int): Set<CheckRequirements.Requirements> {
            return emptySet()
        }
    }
    val usbDevicesMonitor = object: UsbDevicesMonitor {
        override fun getUsbDevices(): List<UsbDevicesMonitor.UsbDevice> {
            TODO("Not yet implemented")
        }

        override fun usbDevices(): Flow<List<UsbDevicesMonitor.UsbDevice>> {
            TODO("Not yet implemented")
        }

        override fun attached(): Flow<List<UsbDevicesMonitor.UsbDevice>> {
            TODO("Not yet implemented")
        }

        override fun detached(): Flow<List<UsbDevicesMonitor.UsbDevice>> {
            TODO("Not yet implemented")
        }

        override fun startSession(): Int {
            TODO("Not yet implemented")
        }

        override fun stopSesstion(sessionId: Int) {
            TODO("Not yet implemented")
        }

        override fun forceReload() {
            TODO("Not yet implemented")
        }
    }
    val checkRequirementsNoUsb = object: CheckRequirements {
        override fun invoke(usbDeviceId: Int): Set<CheckRequirements.Requirements> {
            return setOf(CheckRequirements.Requirements.USB_DEVICE_PERMISSION_REQUIRED)
        }
    }
    val checkRequirementsNoCamera = object: CheckRequirements {
        override fun invoke(usbDeviceId: Int): Set<CheckRequirements.Requirements> {
            return setOf(CheckRequirements.Requirements.CAMERA_PERMISSION_REQUIRED)
        }
    }
    val jpegBenchmark = object: JpegBenchmark {
        override fun getDecoderName(): String {
            TODO("Not yet implemented")
        }
        override fun startBenchmark(args: JpegBenchmark.Arguments):
                SharedFlow<JpegBenchmark.JpegBenchmarkProgress> {
            TODO("Not yet implemented")
        }
    }
    val testUsbDevice = UsbDevicesMonitor.UsbDevice(
        usbDeviceId = 1,
        displayName = "Test Device",
        vendorName = "Test Vendor",
        classesStr = "Test Class"
    )

    @Test
    fun tryOpenDeviceAllGranted() {
        val vm = DeviceListViewModel(
            jpegBenchmark = jpegBenchmark,
            checkRequirements =checkRequirementsAllGranted,
            usbDevicesMonitor = usbDevicesMonitor
        )
        vm.onClick(testUsbDevice)
        val state = vm.state.value
        Assert.assertTrue(state.openPreviewDevice)
        Assert.assertEquals(testUsbDevice.usbDeviceId, state.selectedDeviceId)
    }

    @Test
    fun tryOpenDeviceNoUsb() {
        val vm = DeviceListViewModel(
            jpegBenchmark = jpegBenchmark,
            checkRequirements = checkRequirementsNoUsb,
            usbDevicesMonitor = usbDevicesMonitor
        )
        vm.onClick(testUsbDevice)
        var state = vm.state.value
        Assert.assertTrue(state.informUserAboutPermissions)
        Assert.assertEquals(testUsbDevice.usbDeviceId, state.selectedDeviceId)
        vm.onUserInformedAboutPermission()
        state = vm.state.value
        Assert.assertTrue(state.requestUsbDevicePermission)
        Assert.assertEquals(testUsbDevice.usbDeviceId, state.selectedDeviceId)
    }

    @Test
    fun tryOpenDeviceNoCamera() {
        val vm = DeviceListViewModel(
            jpegBenchmark = jpegBenchmark,
            checkRequirements = checkRequirementsNoCamera,
            usbDevicesMonitor = usbDevicesMonitor
        )
        vm.onClick(testUsbDevice)
        var state = vm.state.value
        Assert.assertTrue(state.informUserAboutPermissions)
        Assert.assertEquals(testUsbDevice.usbDeviceId, state.selectedDeviceId)
        vm.onUserInformedAboutPermission()
        state = vm.state.value
        Assert.assertTrue(state.requestCameraPermission)
        Assert.assertEquals(testUsbDevice.usbDeviceId, state.selectedDeviceId)
    }
}