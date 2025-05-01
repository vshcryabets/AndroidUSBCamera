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

import android.content.Intent
import android.hardware.usb.UsbManager
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.toArgb
import androidx.lifecycle.Lifecycle
import androidx.lifecycle.ViewModelProvider
import androidx.lifecycle.lifecycleScope
import androidx.lifecycle.repeatOnLifecycle
import com.example.cupcake.ui.theme.AusbcTheme
import com.jiangdg.demo.MainActivity
import com.vsh.screens.AusbcApp
import com.vsh.screens.DeviceListViewModel
import com.vsh.screens.DeviceListViewModelFactory
import com.vsh.uvc.JpegBenchmark
import kotlinx.coroutines.launch
import timber.log.Timber


class DevicesActivity : ComponentActivity() {

    lateinit var viewModel: DeviceListViewModel

    override fun onCreate(savedInstanceState: Bundle?) {
        enableEdgeToEdge()
        super.onCreate(savedInstanceState)
        getWindow().getDecorView().setBackgroundColor(Color.White.toArgb())
        viewModel = ViewModelProvider(
            this, DeviceListViewModelFactory(
                usbManager = applicationContext.getSystemService(USB_SERVICE) as UsbManager,
                jpegBenchmark = JpegBenchmark(
                    context = applicationContext
                )
            )
        ).get(DeviceListViewModel::class.java)
        setContent {
            AusbcTheme {
                AusbcApp(viewModel = viewModel)
            }
        }
    }

    override fun onResume() {
        super.onResume()
        viewModel.begin()

        lifecycleScope.launch {
            viewModel.state.collect {
                if (it.openPreviewDeviceId != null) {
                    viewModel.onPreviewOpened()
                    val intent =
                        MainActivity.newInstance(applicationContext, it.openPreviewDeviceId)
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
        viewModel.stop()
        super.onPause()
    }

}