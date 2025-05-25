package com.jiangdg.demo

import android.app.PendingIntent
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.hardware.usb.UsbManager
import android.os.Build
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.fragment.app.Fragment
import timber.log.Timber

class DemoFragment2: Fragment() {

    private val usbReceiver = object : BroadcastReceiver() {
        override fun onReceive(context: Context, intent: Intent) {
            Timber.d("ASD got message $intent")
        }
    }

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        return inflater.inflate(R.layout.fragment_demo2, container, false)
    }

    override fun onResume() {
        super.onResume()
        val deviceId = requireArguments().getInt(KEY_USB_DEVICE, -1)
        val usbManager = requireContext().getSystemService(UsbManager::class.java)
        val device = usbManager.deviceList.values.firstOrNull { it.deviceId == deviceId }
            ?: throw IllegalStateException("Can't find USB device $deviceId")

        val pendingIntent : PendingIntent
        if (Build.VERSION.SDK_INT >= 31) {
            pendingIntent = PendingIntent.getBroadcast(
                requireContext(),
                0,
                Intent(ACTION_USB_PERMISSION),
                PendingIntent.FLAG_IMMUTABLE
            )
        } else {
            pendingIntent =
                PendingIntent.getBroadcast(requireContext(), 0, Intent(ACTION_USB_PERMISSION), 0)
        }
        val filter = IntentFilter(ACTION_USB_PERMISSION)
        filter.addAction(UsbManager.ACTION_USB_DEVICE_DETACHED)
        requireContext().registerReceiver(usbReceiver, filter, Context.RECEIVER_EXPORTED)

        usbManager.requestPermission(device, pendingIntent);
    }

    override fun onPause() {
        requireContext().unregisterReceiver(usbReceiver)
        super.onPause()
    }

    companion object {
        const val KEY_USB_DEVICE = "usbDeviceId"
        const val ACTION_USB_PERMISSION: String = "com.jiangdg.demo.USB_PERMISSION"

        fun newInstance(usdDeviceId: Int): DemoFragment2 {
            val args = Bundle()
            args.putInt(KEY_USB_DEVICE, usdDeviceId)
            val fragment = DemoFragment2()
            fragment.arguments = args
            return fragment
        }
    }
}