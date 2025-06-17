/*
 * Copyright 2024-2025 vschryabets@gmail.com
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

package com.vsh.screens

import androidx.lifecycle.ViewModel
import androidx.lifecycle.ViewModelProvider
import androidx.lifecycle.viewModelScope
import com.jiangdg.demo.BuildConfig
import com.vsh.uvc.CheckRequirements
import com.vsh.uvc.JpegBenchmark
import com.vsh.uvc.LoadUsbDevices
import kotlinx.coroutines.Job
import kotlinx.coroutines.delay
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.update
import kotlinx.coroutines.launch
import timber.log.Timber


data class DeviceListViewState(
    val devices: List<LoadUsbDevices.UsbDevice> = emptyList(),
    val openPreviewDevice: Boolean = false,
    val selectedDeviceId: Int = 0,
    val requestCameraPermission: Boolean = false,
    val requestUsbDevicePermission: Boolean = false,
    val userInformedAboutPermission: Boolean = false,
    val informUserAboutPermissions: Boolean = false,
    val cantOpenWithoutCameraPermission: Boolean = false,
)

data class BenchmarkState(
    val isRunning: Boolean = false,
    val text: String = "",
    val needToShareText: Boolean = false
)

class DeviceListViewModelFactory(
    private val jpegBenchmark: JpegBenchmark,
    private val checkRequirements: CheckRequirements,
    private val loadUsbDevices: LoadUsbDevices,
) : ViewModelProvider.Factory {
    @Suppress("UNCHECKED_CAST")
    override fun <T : ViewModel> create(modelClass: Class<T>): T =
        DeviceListViewModel(
            jpegBenchmark = jpegBenchmark,
            checkRequirements = checkRequirements,
            loadUsbDevices = loadUsbDevices
        ) as T
}

class DeviceListViewModel(
    private val jpegBenchmark: JpegBenchmark,
    private val checkRequirements: CheckRequirements,
    private val loadUsbDevices: LoadUsbDevices,
) : ViewModel() {
    private val _state = MutableStateFlow(
        DeviceListViewState()
    )
    private val _benchmarkState = MutableStateFlow(BenchmarkState())
    private var loadDevicesJob: Job? = null
    private var isActive = false

    val state: StateFlow<DeviceListViewState> = _state
    val benchmarkState: StateFlow<BenchmarkState> = _benchmarkState

    /**
     * Starts a periodic update loop to refresh the list of USB devices.
     * The loop runs every second while `isActive` is true, calling `loadDevices()`
     * to fetch the latest device information. This ensures the UI remains up-to-date
     * with the current state of connected USB devices.
     */
    fun begin() {
        if (!isActive) {
            loadDevicesJob?.cancel()
            loadDevicesJob = viewModelScope.launch {
                isActive = true
                while (isActive) {
                    onEnumerate()
                    delay(1000L)
                }
            }
        }
    }

    fun stop() {
        isActive = false
        loadDevicesJob?.cancel()
        loadDevicesJob = null
    }

    fun onEnumerate() {
        _state.update {
            it.copy(
                devices = loadUsbDevices.load()
            )
        }
    }

    fun onUserInformedAboutPermission() {
        _state.update {
            it.copy(
                userInformedAboutPermission = true,
                informUserAboutPermissions = false
            )
        }
        tryOpenDevice()
    }

    fun tryOpenDevice() {
        val usbDeviceId = state.value.selectedDeviceId
        Timber.i("tryOpenDevice usbDeviceId $usbDeviceId")
        val requirements = checkRequirements(usbDeviceId)
        if (requirements.isNotEmpty()) {
            Timber.i("To open device required: $requirements")
            if (state.value.userInformedAboutPermission) {
                _state.update {
                    it.copy(
                        requestCameraPermission =
                            requirements.contains(CheckRequirements.Requirements.CAMERA_PERMISSION_REQUIRED),
                        requestUsbDevicePermission =
                            requirements.contains(CheckRequirements.Requirements.USB_DEVICE_PERMISSION_REQUIRED)
                    )
                }
            } else {
                _state.update {
                    it.copy(
                        informUserAboutPermissions = true,
                    )
                }
            }

        } else {
            _state.update { it.copy(openPreviewDevice = true) }
        }
    }

    fun onClick(device: LoadUsbDevices.UsbDevice) {
        _state.update { it.copy(selectedDeviceId = device.usbDeviceId) }
        tryOpenDevice()
    }

    fun onShareBenchmarkResults() {
        val text = _benchmarkState.value.text
        _benchmarkState.update { it.copy(needToShareText = true) }
    }

    fun onShareBenchmarkResultsDismissed() =
        _benchmarkState.update { it.copy(needToShareText = false) }

    fun onBenchmarks() {
        val iterations = 30
        val decoderName = jpegBenchmark.getDecoderName()
        _benchmarkState.update { it.copy(isRunning = true) }
        val flow = jpegBenchmark.startBenchmark(
            JpegBenchmark.Arguments(
                imageSamples = listOf(
                    Pair(10360, "jpeg_samples/sample1_0360.jpg"),
                    Pair(10480, "jpeg_samples/sample1_0480.jpg"),
                    Pair(10720, "jpeg_samples/sample1_0720.jpg"),
                    Pair(11080, "jpeg_samples/sample1_1080.jpg"),
                    Pair(11440, "jpeg_samples/sample1_1440.jpg"),
                    Pair(12160, "jpeg_samples/sample1_2160.jpg"),
                    Pair(20360, "jpeg_samples/sample2_0360.jpg"),
                    Pair(20480, "jpeg_samples/sample2_0480.jpg"),
                    Pair(20720, "jpeg_samples/sample2_0720.jpg"),
                    Pair(21080, "jpeg_samples/sample2_1080.jpg"),
                    Pair(21440, "jpeg_samples/sample2_1440.jpg"),
                    Pair(22160, "jpeg_samples/sample2_2160.jpg"),
                    Pair(30360, "jpeg_samples/sample3_0360.jpg"),
                    Pair(30480, "jpeg_samples/sample3_0480.jpg"),
                    Pair(30720, "jpeg_samples/sample3_0720.jpg"),
                    Pair(31080, "jpeg_samples/sample3_1080.jpg"),
                    Pair(31440, "jpeg_samples/sample3_1440.jpg"),
                    Pair(32160, "jpeg_samples/sample3_2160.jpg")
                ),
                iterations = iterations
            )
        )
        viewModelScope.launch {
            flow.collect { progress ->
                if (progress.completed) {
                    val builder = StringBuilder()
                    builder.append("Complete time ${progress.totalTime} ms, iterations=$iterations\n")
                    builder.append("Build type: ${if (BuildConfig.DEBUG) "Debug" else "Release"}\n")
                    builder.append("Device: ${android.os.Build.MODEL} (${android.os.Build.DEVICE})\n")
                    builder.append("JPEG decoder: ${decoderName})\n")
                    progress.results.forEach {
                        val perFrame = it.second / iterations
                        builder.append("Sample id ${it.first}, time ${perFrame} ms\n")
                    }
                    _benchmarkState.update { it.copy(isRunning = false, text = builder.toString()) }
                } else {
                    _benchmarkState.update { it.copy(text = "Sample id ${progress.currentSampleNumber}") }
                }
            }
        }
    }

    fun onCameraPermissionGranted() {
        tryOpenDevice()
    }

    fun onCameraPermissionDenied() {
        Timber.d("Camera permission denied")
        _state.update {
            it.copy(
                cantOpenWithoutCameraPermission = true,
                selectedDeviceId = 0
            )
        }
    }

    fun onCantOpenShown() {
        _state.update { it.copy(cantOpenWithoutCameraPermission = false) }
    }

    fun onCameraPermissionRequested() {
        _state.update { it.copy(requestCameraPermission = false) }
    }

    fun onUsbDevicePermissionRequested() {
        _state.update { it.copy(requestUsbDevicePermission = false) }
    }

    fun onPreviewOpened() {
        _state.update { it.copy(openPreviewDevice = false) }
    }

    fun onUsbDevicePermissionResult(usbDeviceId: Int, permissionGranted: Boolean) {
        if (usbDeviceId == state.value.selectedDeviceId) {
            if (permissionGranted) {
                Timber.i("USB device permission granted for device $usbDeviceId")
                tryOpenDevice()
            } else {
                Timber.w("USB device permission denied for device $usbDeviceId")
                _state.update { it.copy(selectedDeviceId = 0) }
            }
        } else {
            Timber.w("USB device permission result for unknown device $usbDeviceId")
        }
    }

    fun onUsbDeviceDetached(usbDeviceId: Int) {
        Timber.d("USB device detached: $usbDeviceId")
    }
}
