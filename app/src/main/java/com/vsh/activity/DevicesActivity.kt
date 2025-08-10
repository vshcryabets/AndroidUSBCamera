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
package com.vsh.activity

import android.app.PendingIntent
import android.content.BroadcastReceiver
import android.content.Context
import android.content.ContextWrapper
import android.content.Intent
import android.content.IntentFilter
import android.hardware.usb.UsbDevice
import android.hardware.usb.UsbManager
import android.os.Build
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.toArgb
import androidx.lifecycle.ViewModelProvider
import androidx.lifecycle.lifecycleScope
import com.example.cupcake.ui.theme.AusbcTheme
import com.jiangdg.ausbc.utils.CheckCameraPermissionUseCaseImpl
import com.jiangdg.demo.MainActivity
import com.vsh.domain.usecases.GetTestSourceUseCaseImpl
import com.vsh.font.FontSrcImpl
import com.vsh.screens.AusbcApp
import com.vsh.screens.DeviceListViewModel
import com.vsh.screens.DeviceListViewModelFactory
import com.vsh.screens.TestSourceViewModelFactory
import com.vsh.uvc.CheckRequirementsImpl
import com.vsh.uvc.JpegBenchmarkImpl
import com.vsh.uvc.UsbDeviceMonitorImpl
import kotlinx.coroutines.launch
import timber.log.Timber

class DevicesActivity : ComponentActivity() {

    lateinit var viewModel: DeviceListViewModel
    val contextWrapper = ContextWrapper(this)

    private val usbReceiver = object : BroadcastReceiver() {
        override fun onReceive(context: Context, intent: Intent) {
            if (intent.action == ACTION_USB_PERMISSION) {
                val device = intent.getParcelableExtra<UsbDevice>(UsbManager.EXTRA_DEVICE)
                val granted = intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED, false)
                if (device != null && granted) {
                    viewModel.onUsbDevicePermissionResult(device.deviceId, true)
                } else {
                    viewModel.onUsbDevicePermissionResult(0, false)
                }
            }
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        enableEdgeToEdge()
        super.onCreate(savedInstanceState)
        getWindow().getDecorView().setBackgroundColor(Color.White.toArgb())
        viewModel = ViewModelProvider(
            this, DeviceListViewModelFactory(
                usbDevicesMonitor = UsbDeviceMonitorImpl(
                    usbManager = applicationContext.getSystemService(USB_SERVICE) as UsbManager,
                    contextWrapper = contextWrapper
                ),
                jpegBenchmark = JpegBenchmarkImpl(),
                checkRequirements = CheckRequirementsImpl(
                    checkCameraPermissionUseCase = CheckCameraPermissionUseCaseImpl(
                        applicationContext
                    ),
                    usbManager = applicationContext.getSystemService(USB_SERVICE) as UsbManager
                )
            )
        ).get(DeviceListViewModel::class.java)
        setContent {
            AusbcTheme {
                AusbcApp(
                    viewModel = viewModel,
                    testSourceViewModelFactory = TestSourceViewModelFactory(
                        getTestSourceUseCase = GetTestSourceUseCaseImpl(
                            fontSrc = FontSrcImpl()
                        )
                    )
                )
            }
        }
    }

    override fun onResume() {
        super.onResume()
        val filter = IntentFilter(ACTION_USB_PERMISSION)
        contextWrapper.registerReceiver(usbReceiver, filter, Context.RECEIVER_EXPORTED)
        viewModel.begin()

        lifecycleScope.launch {
            viewModel.state.collect { state ->
                if (state.requestCameraPermission) {
                    viewModel.onCameraPermissionRequested()
                    requestPermissions(
                        arrayOf(android.Manifest.permission.CAMERA),
                        CAMERA_PERMISSION_REQUEST_CODE
                    )
                }
                if (state.requestUsbDevicePermission) {
                    viewModel.onUsbDevicePermissionRequested()
                    val usbManager =
                        applicationContext.getSystemService(USB_SERVICE) as UsbManager
                    val usbDevice = usbManager.deviceList.values.firstOrNull {
                        it.deviceId == state.selectedDeviceId
                    }

                    val pendingIntent: PendingIntent
                    if (Build.VERSION.SDK_INT >= 31) {
                        pendingIntent = PendingIntent.getBroadcast(
                            this@DevicesActivity,
                            0,
                            Intent(ACTION_USB_PERMISSION),
                            PendingIntent.FLAG_IMMUTABLE
                        )
                    } else {
                        pendingIntent =
                            PendingIntent.getBroadcast(
                                this@DevicesActivity, 0, Intent(ACTION_USB_PERMISSION),
                                PendingIntent.FLAG_IMMUTABLE
                            )
                    }

                    usbDevice?.let {
                        usbManager.requestPermission(it, pendingIntent)
                    }

                }
                if (state.openPreviewDevice) {
                    viewModel.onPreviewOpened()
                    val intent =
                        MainActivity.newInstance(applicationContext, state.selectedDeviceId)
                    startActivity(intent)
                }
            }
        }
        lifecycleScope.launch {
            viewModel.benchmarkState.collect {
                if (it.needToShareText) {
                    val shareIntent = Intent(Intent.ACTION_SEND).apply {
                        putExtra(Intent.EXTRA_TEXT, it.text)
                        type = "text/plain"
                    }
                    val chooser = Intent.createChooser(shareIntent, "Share via")
                    startActivity(chooser)
                    viewModel.onShareBenchmarkResultsDismissed()
                }
            }
        }
    }

    override fun onPause() {
        contextWrapper.unregisterReceiver(usbReceiver)
        viewModel.stop()
        super.onPause()
    }

    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<out String>,
        grantResults: IntArray,
        deviceId: Int
    ) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults, deviceId)
        if (requestCode == CAMERA_PERMISSION_REQUEST_CODE) {
            if (permissions.isNotEmpty() && grantResults[0] == android.content.pm.PackageManager.PERMISSION_GRANTED) {
                Timber.d("Camera permission granted")
                viewModel.onCameraPermissionGranted()
            } else {
                Timber.d("Camera permission denied")
                viewModel.onCameraPermissionDenied()
            }
        }
    }

    companion object {
        const val CAMERA_PERMISSION_REQUEST_CODE = 1
        const val ACTION_USB_PERMISSION: String = "com.vsh.activity.USB_PERMISSION"
    }

}