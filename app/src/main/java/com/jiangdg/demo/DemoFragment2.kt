/*
 * Copyright 2025 vshcryabets@gmail.com
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
package com.jiangdg.demo

import android.content.Context
import android.hardware.usb.UsbManager
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.fragment.app.Fragment
import com.vsh.source.UVCCamera2

class DemoFragment2 : Fragment() {

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        return inflater.inflate(R.layout.fragment_demo2, container, false)
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        val usbManager = requireActivity().getSystemService(Context.USB_SERVICE) as UsbManager
        val deviceId = requireArguments().getInt(KEY_USB_DEVICE)
        val device = usbManager.deviceList.values.find { it.deviceId == deviceId } ?: return
        val uvcCamera = UVCCamera2()
//        uvcCamera.open()
    }

    companion object {
        val KEY_USB_DEVICE = "usbDeviceId"
        fun newInstance(usbDeviceId: Int): DemoFragment2 {
            val fragment = DemoFragment2()
            fragment.arguments = Bundle().apply {
                putInt(KEY_USB_DEVICE, usbDeviceId)
            }
            return fragment
        }
    }
}
