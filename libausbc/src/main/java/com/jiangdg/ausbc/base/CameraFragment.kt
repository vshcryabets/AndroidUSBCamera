/*
 * Copyright 2017-2023 Jiangdg
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
package com.jiangdg.ausbc.base

import android.content.Context
import android.graphics.SurfaceTexture
import android.hardware.usb.UsbDevice
import android.hardware.usb.UsbManager
import android.view.Gravity
import android.view.SurfaceHolder
import android.view.SurfaceView
import android.view.TextureView
import android.view.ViewGroup
import android.widget.FrameLayout
import android.widget.LinearLayout
import android.widget.RelativeLayout
import androidx.activity.ComponentActivity
import com.jiangdg.ausbc.MultiCameraClient
import com.jiangdg.ausbc.callback.ICameraStateCallBack
import com.jiangdg.ausbc.callback.IDeviceConnectCallBack
import com.jiangdg.ausbc.camera.CameraUVC
import com.jiangdg.ausbc.camera.bean.CameraRequest
import com.jiangdg.ausbc.camera.bean.PreviewSize
import com.jiangdg.ausbc.render.env.RotateType
import com.jiangdg.ausbc.utils.SettableFuture
import com.jiangdg.ausbc.widget.IAspectRatio
import com.jiangdg.usb.USBMonitor
import timber.log.Timber
import java.util.concurrent.TimeUnit
import java.util.concurrent.atomic.AtomicBoolean

/**Extends from BaseFragment for one uvc camera
 *
 * @author Created by jiangdg on 2023/2/3
 */
abstract class CameraFragment : BaseFragment(), ICameraStateCallBack {
    private var mCameraView: IAspectRatio? = null
    private var mCameraClient: MultiCameraClient? = null
    private val mCameraMap = hashMapOf<Int, MultiCameraClient.ICamera>()
    private var mCurrentCamera: SettableFuture<MultiCameraClient.ICamera>? = null

    private val mRequestPermission: AtomicBoolean by lazy {
        AtomicBoolean(false)
    }

    open fun getSelectedDeviceId(): Int = -1

    override fun initView() {
        when (val cameraView = getCameraView()) {
            is TextureView -> {
                handleTextureView(cameraView)
                cameraView
            }

            is SurfaceView -> {
                handleSurfaceView(cameraView)
                cameraView
            }

            else -> {
                null
            }
        }.apply {
            mCameraView = this
            // offscreen render
            if (this == null) {
                registerMultiCamera(getSelectedDeviceId())
                return
            }
        }?.also { view ->
            getCameraViewContainer()?.apply {
                removeAllViews()
                addView(view, getViewLayoutParams(this))
            }
        }
    }

    override fun clear() {
        unRegisterMultiCamera()
    }

    protected fun registerMultiCamera(deviceId: Int) {
        mCameraClient = MultiCameraClient(requireContext(), object : IDeviceConnectCallBack {

            override fun onDetachDec(device: UsbDevice?) {
                mCameraMap.remove(device?.deviceId)?.apply {
                    setUsbControlBlock(null)
                }
                mRequestPermission.set(false)
                try {
                    mCurrentCamera?.cancel(true)
                    mCurrentCamera = null
                } catch (e: Exception) {
                    e.printStackTrace()
                }
            }

            override fun onConnectDev(device: UsbDevice?, ctrlBlock: USBMonitor.UsbControlBlock?) {
                device ?: return
                ctrlBlock ?: return
                context ?: return
                mCameraMap[device.deviceId]?.apply {
                    setUsbControlBlock(ctrlBlock)
                }?.also { camera ->
                    try {
                        mCurrentCamera?.cancel(true)
                        mCurrentCamera = null
                    } catch (e: Exception) {
                        e.printStackTrace()
                    }
                    mCurrentCamera = SettableFuture()
                    mCurrentCamera?.set(camera)
                    openCamera(mCameraView)
                    Timber.i("camera connection. pid: ${device.productId}, vid: ${device.vendorId}")
                }
            }

            override fun onDisConnectDec(device: UsbDevice?, ctrlBlock: USBMonitor.UsbControlBlock?) {
                closeCamera()
                mRequestPermission.set(false)
            }

            override fun onCancelDev(device: UsbDevice?) {
                mRequestPermission.set(false)
                try {
                    mCurrentCamera?.cancel(true)
                    mCurrentCamera = null
                } catch (e: Exception) {
                    e.printStackTrace()
                }
            }
        })
        mCameraClient?.register()
        openCamera(deviceId)
    }

    fun openCamera(deviceId: Int) {
        val usbManager = requireActivity().getSystemService(ComponentActivity.USB_SERVICE) as UsbManager
        val device = usbManager.deviceList.values.find { it.deviceId == deviceId } ?: return
        context?.let {
            if (mCameraMap.containsKey(device.deviceId)) {
                return
            }
            generateCamera(it, device).apply {
                mCameraMap[device.deviceId] = this
            }
            // Initiate permission request when device insertion is detected
            // If you want to open the specified camera, you need to override getDefaultCamera()
            if (mRequestPermission.get()) {
                return@let
            }
            getDefaultCamera()?.apply {
                if (vendorId == device.vendorId && productId == device.productId) {
                    Timber.i("default camera pid: $productId, vid: $vendorId")
                    requestPermission(device)
                }
                return@let
            }
            requestPermission(device)
        }
    }

    protected fun unRegisterMultiCamera() {
        mCameraMap.values.forEach {
            it.closeCamera()
        }
        mCameraMap.clear()
        mCameraClient?.unRegister()
        mCameraClient?.destroy()
        mCameraClient = null
    }

    private fun handleTextureView(textureView: TextureView) {
        textureView.surfaceTextureListener = object : TextureView.SurfaceTextureListener {
            override fun onSurfaceTextureAvailable(
                surface: SurfaceTexture,
                width: Int,
                height: Int
            ) {
                registerMultiCamera(getSelectedDeviceId())
            }

            override fun onSurfaceTextureSizeChanged(
                surface: SurfaceTexture,
                width: Int,
                height: Int
            ) {
                surfaceSizeChanged(width, height)
            }

            override fun onSurfaceTextureDestroyed(surface: SurfaceTexture): Boolean {
                unRegisterMultiCamera()
                return false
            }

            override fun onSurfaceTextureUpdated(surface: SurfaceTexture) {
            }
        }
    }

    private fun handleSurfaceView(surfaceView: SurfaceView) {
        surfaceView.holder.addCallback(object : SurfaceHolder.Callback {
            override fun surfaceCreated(holder: SurfaceHolder) {
                registerMultiCamera(getSelectedDeviceId())
            }

            override fun surfaceChanged(
                holder: SurfaceHolder,
                format: Int,
                width: Int,
                height: Int
            ) {
                surfaceSizeChanged(width, height)
            }

            override fun surfaceDestroyed(holder: SurfaceHolder) {
                unRegisterMultiCamera()
            }
        })
    }

    /**
     * Get current opened camera
     *
     * @return current camera, see [MultiCameraClient.ICamera]
     */
    protected fun getCurrentCamera(): MultiCameraClient.ICamera? {
        return try {
            mCurrentCamera?.get(2, TimeUnit.SECONDS)
        } catch (e: Exception) {
            e.printStackTrace()
            null
        }
    }

    /**
     * Request permission
     *
     * @param device see [UsbDevice]
     */
    protected fun requestPermission(device: UsbDevice?) {
        mRequestPermission.set(true)
        mCameraClient?.requestPermission(device)
    }

    /**
     * Generate camera
     *
     * @param ctx context [Context]
     * @param device Usb device, see [UsbDevice]
     * @return Inheritor assignment camera api policy
     */
    protected open fun generateCamera(ctx: Context, device: UsbDevice): MultiCameraClient.ICamera {
        return CameraUVC(ctx, device)
    }

    /**
     * Get default camera
     *
     * @return Open camera by default, should be [UsbDevice]
     */
    protected open fun getDefaultCamera(): UsbDevice? = null

    /**
     * Switch camera
     *
     * @param usbDevice camera usb device
     */
    protected fun switchCamera(usbDevice: UsbDevice) {
        getCurrentCamera()?.closeCamera()
        try {
            Thread.sleep(500)
        } catch (e: Exception) {
            e.printStackTrace()
        }
        requestPermission(usbDevice)
    }

    /**
     * Update resolution
     *
     * @param width camera preview width
     * @param height camera preview height
     */
    protected fun updateResolution(width: Int, height: Int) {
        getCurrentCamera()?.updateResolution(width, height)
    }

    /**
     * Get all preview sizes
     *
     * @param aspectRatio preview size aspect ratio,
     *                      null means getting all preview sizes
     */
    protected fun getAllPreviewSizes(aspectRatio: Double? = null): MutableList<PreviewSize>? {
        if (getCurrentCamera() == null) {
            Timber.e("getAllPreviewSizes camera is null")
        }
        return getCurrentCamera()?.getAllPreviewSizes(aspectRatio)
    }

    /**
     * Get current preview size
     *
     * @return camera preview size, see [PreviewSize]
     */
    protected fun getCurrentPreviewSize(): PreviewSize? {
        return getCurrentCamera()?.getCameraRequest()?.let {
            PreviewSize(it.previewWidth, it.previewHeight)
        }
    }

    protected fun openCamera(st: IAspectRatio? = null) {
        when (st) {
            is TextureView, is SurfaceView -> {
                st
            }

            else -> {
                null
            }
        }.apply {
            getCurrentCamera()?.openCamera(this, getCameraRequest())
            getCurrentCamera()?.setCameraStateCallBack(this@CameraFragment)
        }
    }

    protected fun closeCamera() {
        getCurrentCamera()?.closeCamera()
    }

    private fun surfaceSizeChanged(surfaceWidth: Int, surfaceHeight: Int) {
        getCurrentCamera()?.setRenderSize(surfaceWidth, surfaceHeight)
    }

    private fun getViewLayoutParams(viewGroup: ViewGroup): ViewGroup.LayoutParams {
        return when (viewGroup) {
            is FrameLayout -> {
                FrameLayout.LayoutParams(
                    FrameLayout.LayoutParams.MATCH_PARENT,
                    FrameLayout.LayoutParams.MATCH_PARENT,
                    getGravity()
                )
            }

            is LinearLayout -> {
                LinearLayout.LayoutParams(
                    LinearLayout.LayoutParams.MATCH_PARENT,
                    LinearLayout.LayoutParams.MATCH_PARENT
                ).apply {
                    gravity = getGravity()
                }
            }

            is RelativeLayout -> {
                RelativeLayout.LayoutParams(
                    RelativeLayout.LayoutParams.MATCH_PARENT,
                    RelativeLayout.LayoutParams.MATCH_PARENT
                ).apply {
                    when (getGravity()) {
                        Gravity.TOP -> {
                            addRule(RelativeLayout.ALIGN_PARENT_TOP, RelativeLayout.TRUE)
                        }

                        Gravity.BOTTOM -> {
                            addRule(RelativeLayout.ALIGN_PARENT_BOTTOM, RelativeLayout.TRUE)
                        }

                        else -> {
                            addRule(RelativeLayout.CENTER_HORIZONTAL, RelativeLayout.TRUE)
                            addRule(RelativeLayout.CENTER_VERTICAL, RelativeLayout.TRUE)
                        }
                    }
                }
            }

            else -> throw IllegalArgumentException(
                "Unsupported container view, " +
                        "you can use FrameLayout or LinearLayout or RelativeLayout"
            )
        }
    }

    /**
     * Get camera view
     *
     * @return CameraView, such as AspectRatioTextureView etc.
     */
    protected abstract fun getCameraView(): IAspectRatio?

    /**
     * Get camera view container
     *
     * @return camera view container, such as FrameLayout ect
     */
    protected abstract fun getCameraViewContainer(): ViewGroup?

    /**
     * Camera render view show gravity
     */
    protected open fun getGravity() = Gravity.CENTER

    protected open fun getCameraRequest(): CameraRequest {
        return CameraRequest.Builder()
            .setPreviewWidth(640)
            .setPreviewHeight(480)
            .setRenderMode(CameraRequest.RenderMode.OPENGL)
            .setDefaultRotateType(RotateType.ANGLE_0)
            .setAudioSource(CameraRequest.AudioSource.SOURCE_SYS_MIC)
            .setPreviewFormat(CameraRequest.PreviewFormat.FORMAT_MJPEG)
            .setAspectRatioShow(true)
            .setCaptureRawImage(false)
            .setRawPreviewData(false)
            .create()
    }
}