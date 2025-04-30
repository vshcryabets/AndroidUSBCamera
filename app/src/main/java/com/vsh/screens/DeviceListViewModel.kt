/*
 * Copyright 2024 vschryabets@gmail.com
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

import android.content.Intent
import android.hardware.usb.UsbConstants
import android.hardware.usb.UsbManager
import androidx.lifecycle.ViewModel
import androidx.lifecycle.ViewModelProvider
import androidx.lifecycle.viewModelScope
import androidx.lifecycle.viewmodel.compose.viewModel
import com.jiangdg.demo.BuildConfig
import com.jiangdg.usb.USBVendorId
import com.vsh.uvc.JpegBenchmark
import kotlinx.coroutines.Job
import kotlinx.coroutines.delay
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.update
import kotlinx.coroutines.launch
import timber.log.Timber

data class UsbDevice(
    val usbDevcieId: Int,
    val displayName: String,
    val vendorName: String,
    val classesStr: String
)

data class DeviceListViewState(
    val devices: List<UsbDevice> = emptyList(),
    val openPreviewDeviceId: Int? = null
)

data class BenchmarkState(
    val isRunning: Boolean = false,
    val text: String = "",
    val needToShareText: Boolean = false
)

class DeviceListViewModelFactory(
    private val usbManager: UsbManager,
    private val jpegBenchmark: JpegBenchmark
): ViewModelProvider.Factory {
    @Suppress("UNCHECKED_CAST")
    override fun <T : ViewModel> create(modelClass: Class<T>): T =
        DeviceListViewModel(
            usbManager = usbManager,
            jpegBenchmark = jpegBenchmark
        ) as T
}

class DeviceListViewModel(
    private val usbManager: UsbManager,
    private val jpegBenchmark: JpegBenchmark
) : ViewModel() {
    private val _state = MutableStateFlow(
        DeviceListViewState()
    )
    private val _benchmarkState = MutableStateFlow(BenchmarkState())
    private var loadDevicesJob : Job? = null
    private var isActive = false

    val state: StateFlow<DeviceListViewState> = _state
    val benchmarkState: StateFlow<BenchmarkState> = _benchmarkState

    fun begin() {
        loadDevicesJob = viewModelScope.launch {
            isActive = true
            while (isActive) {
                loadDevices()
                delay(1000L)
            }
        }
        loadDevices()
    }

    fun stop() {
        isActive = false
        loadDevicesJob?.cancel()
    }

    fun onEnumarate() {
        loadDevices()
    }

    fun loadDevices() {
        val usbDevices = usbManager.deviceList
        _state.update {
            it.copy(
                devices = usbDevices.values.map { device ->
                    val vendorName = USBVendorId.vendorName(device.vendorId)
                    val vidPidStr = String.format("%04x:%04x", device.vendorId, device.productId)
                    val classesList = mutableSetOf<Int>()
                    classesList.add(device.deviceClass)
                    if (device.deviceClass == UsbConstants.USB_CLASS_MISC) {
                        for (i in 0 until device.interfaceCount) {
                            classesList.add(device.getInterface(i).interfaceClass)
                        }
                    }

                    UsbDevice(
                        usbDevcieId = device.deviceId,
                        displayName = "$vidPidStr ${device.deviceName}",
                        vendorName = if (vendorName.isEmpty()) "${device.vendorId}" else vendorName,
                        classesStr = classesList.map{
                            USBVendorId.CLASSES[it] ?: "$it"
                        }.joinToString(",\n")
                    )
                }
            )
        }
    }

    fun onClick(device: UsbDevice) {
        _state.update {
            it.copy(openPreviewDeviceId = device.usbDevcieId)
        }
    }

    fun onPreviewOpened() {
        _state.update {
            it.copy(openPreviewDeviceId = null)
        }
    }

    fun onShareBenchmarkResults() {
        val text = _benchmarkState.value.text
        _benchmarkState.update { it.copy(needToShareText = true) }
    }

    fun onShareBenchmarkResultsDismissed() =  _benchmarkState.update { it.copy(needToShareText = false) }

    fun onBenchmarks() {
        val iterations = 30
        val decoderName = jpegBenchmark.getDecoderName()
        _benchmarkState.update { it.copy(isRunning = true) }
        val flow = jpegBenchmark.startBenchmark(JpegBenchmark.Arguments(
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
        ))
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
}