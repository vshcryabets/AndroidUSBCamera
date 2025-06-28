/*
 * Copyright 2017-2023 Jiangdg
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
package com.jiangdg.ausbc.camera

import android.content.Context
import android.graphics.SurfaceTexture
import android.hardware.usb.UsbDevice
import android.os.Handler
import android.os.HandlerThread
import android.os.Looper
import android.os.Message
import android.view.Surface
import android.view.SurfaceView
import android.view.TextureView
import com.jiangdg.ausbc.callback.ICameraStateCallBack
import com.jiangdg.ausbc.camera.bean.CameraRequest
import com.jiangdg.ausbc.camera.bean.PreviewSize
import com.jiangdg.ausbc.render.RenderManager
import com.jiangdg.ausbc.utils.CameraUtils
import com.jiangdg.ausbc.utils.CheckCameraPermissionUseCase
import com.jiangdg.ausbc.utils.Logger
import com.jiangdg.ausbc.utils.OpenGLUtils
import com.jiangdg.ausbc.utils.ReadRawTextFileUseCase
import com.jiangdg.ausbc.utils.SettableFuture
import com.jiangdg.ausbc.utils.Utils
import com.jiangdg.ausbc.widget.IAspectRatio
import com.jiangdg.usb.USBMonitor
import com.jiangdg.usb.UsbControlBlock
import com.jiangdg.uvc.UVCCamera
import com.jiangdg.uvc.UvcFrameFormat
import java.util.concurrent.TimeUnit
import kotlin.math.abs

/** UVC Camera
 *
 * @author Created by jiangdg on 2023/1/15
 */
class CameraUVC(
    private val ctx: Context,
    private val device: UsbDevice,
    private val readRawTextFileUseCase: ReadRawTextFileUseCase,
    private val checkCameraPermissionUseCase: CheckCameraPermissionUseCase,
) : Handler.Callback {
    private var mCameraThread: HandlerThread? = null
    protected var mCameraHandler: Handler? = null

    private var mRenderManager: RenderManager ? = null

    private var mCameraView: Any? = null
    private var mCameraStateCallback: ICameraStateCallBack? = null

    private var mSizeChangedFuture: SettableFuture<Pair<Int, Int>>? = null
    protected var mCameraRequest: CameraRequest? = null
    protected var isPreviewed: Boolean = false
    protected var isNeedGLESRender: Boolean = false
    protected var mCtrlBlock: UsbControlBlock? = null
    protected val mMainHandler: Handler by lazy {
        Handler(Looper.getMainLooper())
    }
    private var mUvcCamera: UVCCamera? = null
    private val mCameraPreviewSize by lazy {
        arrayListOf<PreviewSize>()
    }

    /**
     * Get all preview sizes
     *
     * @param aspectRatio aspect ratio
     * @return [PreviewSize] list of camera
     */
    fun getAllPreviewSizes(): List<PreviewSize> {
        val previewSizeList = arrayListOf<PreviewSize>()
        val isMjpegFormat =
            mCameraRequest?.previewFormat == UvcFrameFormat.FRAME_FORMAT_MJPEG
        if (isMjpegFormat && (mUvcCamera?.supportedSizeList2?.isNotEmpty() == true)) {
            mUvcCamera?.supportedSizeList2
        } else {
            mUvcCamera?.getSupportedSizeList2(UvcFrameFormat.FRAME_FORMAT_YUYV)
        }?.let { sizeList ->
            if (sizeList.size > mCameraPreviewSize.size) {
                mCameraPreviewSize.clear()
                sizeList.forEach { size ->
                    val width = size.width
                    val height = size.height
                    mCameraPreviewSize.add(PreviewSize(width, height))
                }
            }
            if (Utils.debugCamera) {
                Logger.i(TAG, "supportedSizeList = $sizeList")
            }
            mCameraPreviewSize
        }?.onEach { size ->
            val width = size.width
            val height = size.height
            val ratio = width.toDouble() / height
            previewSizeList.add(PreviewSize(width, height))
        }
        return previewSizeList
    }

    fun <T> openCameraInternal(cameraView: T) {
        if (Utils.isTargetSdkOverP(ctx) && !checkCameraPermissionUseCase()) {
            closeCamera()
            postStateEvent(ICameraStateCallBack.State.ERROR, "Has no CAMERA permission.")
            Logger.e(
                TAG,
                "open camera failed, need Manifest.permission.CAMERA permission when targetSdk>=28"
            )
            return
        }
        if (mCtrlBlock == null) {
            closeCamera()
            postStateEvent(ICameraStateCallBack.State.ERROR, "Usb control block can not be null ")
            return
        }
        // 1. create a UVCCamera
        val request = mCameraRequest!!
        try {
            mUvcCamera = UVCCamera().apply {
                open(mCtrlBlock!!)
            }
        } catch (e: Exception) {
            closeCamera()
            postStateEvent(
                ICameraStateCallBack.State.ERROR,
                "open camera failed ${e.localizedMessage}"
            )
            Logger.e(TAG, "open camera failed.", e)
        }

        // 2. set preview size and register preview callback
        var previewSize = getSuitableSize(request.previewWidth, request.previewHeight).apply {
            mCameraRequest!!.previewWidth = width
            mCameraRequest!!.previewHeight = height
        }
        val previewFormat = mCameraRequest?.previewFormat
        try {
            Logger.i(TAG, "getSuitableSize: $previewSize")
            if (!isPreviewSizeSupported(previewSize)) {
                closeCamera()
                postStateEvent(ICameraStateCallBack.State.ERROR, "unsupported preview size")
                Logger.e(
                    TAG,
                    "open camera failed, preview size($previewSize) unsupported-> ${mUvcCamera?.supportedSizeList2}"
                )
                return
            }
            // if give custom minFps or maxFps or unsupported preview size
            // this method will fail
            mUvcCamera?.setPreviewSize(
                previewSize.width,
                previewSize.height,
                MIN_FS,
                previewFormat,
                UVCCamera.DEFAULT_BANDWIDTH
            )
        } catch (e: Exception) {
            try {
                previewSize = getSuitableSize(request.previewWidth, request.previewHeight).apply {
                    mCameraRequest!!.previewWidth = width
                    mCameraRequest!!.previewHeight = height
                }
                if (!isPreviewSizeSupported(previewSize)) {
                    postStateEvent(ICameraStateCallBack.State.ERROR, "unsupported preview size")
                    closeCamera()
                    Logger.e(
                        TAG,
                        "open camera failed, preview size($previewSize) unsupported-> ${mUvcCamera?.supportedSizeList2}"
                    )
                    return
                }
                Logger.e(
                    TAG,
                    " setPreviewSize failed(format is $previewFormat), try to use other format..."
                )
                mUvcCamera?.setPreviewSize(
                    previewSize.width,
                    previewSize.height,
                    MIN_FS,
                    if (previewFormat == UvcFrameFormat.FRAME_FORMAT_YUYV) {
                        UvcFrameFormat.FRAME_FORMAT_MJPEG
                    } else {
                        UvcFrameFormat.FRAME_FORMAT_YUYV
                    },
                    UVCCamera.DEFAULT_BANDWIDTH
                )
            } catch (e: Exception) {
                closeCamera()
                postStateEvent(ICameraStateCallBack.State.ERROR, "err: ${e.localizedMessage}")
                Logger.e(TAG, " setPreviewSize failed, even using yuv format", e)
                return
            }
        }
        // 3. start preview
        when (cameraView) {
            is Surface -> {
                mUvcCamera?.setPreviewDisplay(cameraView)
            }

            is SurfaceTexture -> {
                mUvcCamera?.setPreviewTexture(cameraView)
            }

            is SurfaceView -> {
                mUvcCamera?.setPreviewDisplay(cameraView.holder)
            }

            is TextureView -> {
                mUvcCamera?.setPreviewTexture(cameraView.surfaceTexture)
            }

            else -> {
                throw IllegalStateException("Only support Surface or SurfaceTexture or SurfaceView or TextureView or GLSurfaceView--$cameraView")
            }
        }
        mUvcCamera?.autoFocus = true
        mUvcCamera?.autoWhiteBlance = true
        mUvcCamera?.startPreview()
        mUvcCamera?.updateCameraParams()
        isPreviewed = true
        postStateEvent(ICameraStateCallBack.State.OPENED)
        if (Utils.debugCamera) {
            Logger.i(TAG, " start preview, name = ${device.deviceName}, preview=$previewSize")
        }
    }

    fun closeCameraInternal() {
        postStateEvent(ICameraStateCallBack.State.CLOSED)
        isPreviewed = false
        mUvcCamera?.destroy()
        mUvcCamera = null
        if (Utils.debugCamera) {
            Logger.i(TAG, " stop preview, name = ${device.deviceName}")
        }
    }

    /**
     * Is mic supported
     *
     * @return true camera support mic
     */
    fun isMicSupported() = CameraUtils.isCameraContainsMic(this.device)

    /**
     * Send camera command
     *
     * This method cannot be verified, please use it with caution
     */
    fun sendCameraCommand(command: Int) {
        mCameraHandler?.post {
            mUvcCamera?.sendCommand(command)
        }
    }

    /**
     * Set auto focus
     *
     * @param enable true enable auto focus
     */
    fun setAutoFocus(enable: Boolean) {
        mUvcCamera?.autoFocus = enable
    }

    /**
     * Get auto focus
     *
     * @return true enable auto focus
     */
    fun getAutoFocus() = mUvcCamera?.autoFocus

    /**
     * Reset auto focus
     */
    fun resetAutoFocus() {
        mUvcCamera?.resetFocus()
    }

    /**
     * Set auto white balance
     *
     * @param autoWhiteBalance true enable auto white balance
     */
    fun setAutoWhiteBalance(autoWhiteBalance: Boolean) {
        mUvcCamera?.autoWhiteBlance = autoWhiteBalance
    }

    /**
     * Get auto white balance
     *
     * @return true enable auto white balance
     */
    fun getAutoWhiteBalance() = mUvcCamera?.autoWhiteBlance

    /**
     * Set zoom
     *
     * @param zoom zoom value, 0 means reset
     */
    fun setZoom(zoom: Int) {
        mUvcCamera?.zoom = zoom
    }

    /**
     * Get zoom
     */
    fun getZoom() = mUvcCamera?.zoom

    /**
     * Reset zoom
     */
    fun resetZoom() {
        mUvcCamera?.resetZoom()
    }

    /**
     * Set gain
     *
     * @param gain gain value, 0 means reset
     */
    fun setGain(gain: Int) {
        mUvcCamera?.gain = gain
    }

    /**
     * Get gain
     */
    fun getGain() = mUvcCamera?.gain

    /**
     * Reset gain
     */
    fun resetGain() {
        mUvcCamera?.resetGain()
    }

    /**
     * Set gamma
     *
     * @param gamma gamma value, 0 means reset
     */
    fun setGamma(gamma: Int) {
        mUvcCamera?.gamma = gamma
    }

    /**
     * Get gamma
     */
    fun getGamma() = mUvcCamera?.gamma

    /**
     * Reset gamma
     */
    fun resetGamma() {
        mUvcCamera?.resetGamma()
    }

    /**
     * Set brightness
     *
     * @param brightness brightness value, 0 means reset
     */
    fun setBrightness(brightness: Int) {
        mUvcCamera?.brightness = brightness
    }

    /**
     * Get brightness
     */
    fun getBrightness() = mUvcCamera?.brightness

    fun getBrightnessMax() = mUvcCamera?.brightnessMax

    fun getBrightnessMin() = mUvcCamera?.brightnessMin

    /**
     * Reset brightnes
     */
    fun resetBrightness() {
        mUvcCamera?.resetBrightness()
    }

    /**
     * Set contrast
     *
     * @param contrast contrast value, 0 means reset
     */
    fun setContrast(contrast: Int) {
        mUvcCamera?.contrast = contrast
    }

    /**
     * Get contrast
     */
    fun getContrast() = mUvcCamera?.contrast

    /**
     * Reset contrast
     */
    fun resetContrast() {
        mUvcCamera?.resetContrast()
    }

    /**
     * Set sharpness
     *
     * @param sharpness sharpness value, 0 means reset
     */
    fun setSharpness(sharpness: Int) {
        mUvcCamera?.sharpness = sharpness
    }

    /**
     * Get sharpness
     */
    fun getSharpness() = mUvcCamera?.sharpness

    /**
     * Reset sharpness
     */
    fun resetSharpness() {
        mUvcCamera?.resetSharpness()
    }

    /**
     * Set saturation
     *
     * @param saturation saturation value, 0 means reset
     */
    fun setSaturation(saturation: Int) {
        mUvcCamera?.saturation = saturation
    }

    /**
     * Get saturation
     */
    fun getSaturation() = mUvcCamera?.saturation

    /**
     * Reset saturation
     */
    fun resetSaturation() {
        mUvcCamera?.resetSaturation()
    }

    /**
     * Set hue
     *
     * @param hue hue value, 0 means reset
     */
    fun setHue(hue: Int) {
        mUvcCamera?.hue = hue
    }

    /**
     * Get hue
     */
    fun getHue() = mUvcCamera?.hue

    /**
     * Reset saturation
     */
    fun resetHue() {
        mUvcCamera?.resetHue()
    }

    override fun handleMessage(msg: Message): Boolean {
        when (msg.what) {
            MSG_START_PREVIEW -> {
                val previewWidth = mCameraRequest!!.previewWidth
                val previewHeight = mCameraRequest!!.previewHeight
                val renderMode = mCameraRequest!!.renderMode
                when (val cameraView = mCameraView) {
                    is IAspectRatio -> {
                        if (mCameraRequest!!.isAspectRatioShow) {
                            cameraView.setAspectRatio(previewWidth, previewHeight)
                        }
                        cameraView
                    }

                    else -> {
                        null
                    }
                }.also { view ->
                    isNeedGLESRender = isGLESRender(renderMode == CameraRequest.RenderMode.OPENGL)
                    if (!isNeedGLESRender && view != null) {
                        openCameraInternal(view)
                        return true
                    }
                    // use opengl render
                    // if surface is null, force off screen render whatever mode
                    // and use init preview size（measure size） for render size
                    val measureSize = try {
                        mSizeChangedFuture = SettableFuture()
                        mSizeChangedFuture?.get(2000, TimeUnit.MILLISECONDS)
                    } catch (e: Exception) {
                        e.printStackTrace()
                        null
                    }
                    Logger.i(TAG, "surface measure size $measureSize")
                    mCameraRequest!!.renderMode = CameraRequest.RenderMode.OPENGL
                    val screenWidth = view?.getSurfaceWidth() ?: previewWidth
                    val screenHeight = view?.getSurfaceHeight() ?: previewHeight
                    val surface = view?.getSurface()
                    val previewCb = null
                    mRenderManager = RenderManager(
                        surfaceWidth = previewWidth,
                        surfaceHeight = previewHeight,
                        mPreviewDataCbList = previewCb,
                        readRawTextFileUseCase = readRawTextFileUseCase
                    )
                    mRenderManager?.startRenderScreen(
                        screenWidth,
                        screenHeight,
                        surface,
                        object : RenderManager.CameraSurfaceTextureListener {
                            override fun onSurfaceTextureAvailable(surfaceTexture: SurfaceTexture?) {
                                if (surfaceTexture == null) {
                                    closeCamera()
                                    postStateEvent(
                                        ICameraStateCallBack.State.ERROR,
                                        "create camera surface failed"
                                    )
                                    return
                                }
                                openCameraInternal(surfaceTexture)
                            }
                        })
                    mRenderManager?.setRotateType(mCameraRequest!!.defaultRotateType)
                }
            }

            MSG_STOP_PREVIEW -> {
                try {
                    mSizeChangedFuture?.cancel(true)
                    mSizeChangedFuture = null
                } catch (e: Exception) {
                    e.printStackTrace()
                }
                closeCameraInternal()
                mRenderManager?.stopRenderScreen()
                mRenderManager = null
            }
        }
        return true
    }

    /**
     * should use opengl, recommend
     *
     * @return default depend on device opengl version, >=2.0 is true
     */
    private fun isGLESRender(isGlesRenderOpen: Boolean): Boolean =
        isGlesRenderOpen && OpenGLUtils.isGlEsSupported(ctx)

    /**
     * Set render size
     *
     * @param width surface width
     * @param height surface height
     */
    fun setRenderSize(width: Int, height: Int) {
        mSizeChangedFuture?.set(Pair(width, height))
    }

    /**
     * Post camera state to main thread
     *
     * @param state see [ICameraStateCallBack.State]
     * @param msg detail msg
     */
    protected fun postStateEvent(state: ICameraStateCallBack.State, msg: String? = null) {
        mMainHandler.post {
            mCameraStateCallback?.onCameraState(this, state, msg)
        }
    }

    /**
     * Set usb control block, when the uvc device was granted permission
     *
     * @param ctrlBlock see [USBMonitor.OnDeviceConnectListener]#onConnectedDev
     */
    fun setUsbControlBlock(ctrlBlock: UsbControlBlock?) {
        this.mCtrlBlock = ctrlBlock
    }


    /**
     * Open camera
     *
     * @param cameraView render surface view，support Surface or SurfaceTexture
     *                      or SurfaceView or TextureView or GLSurfaceView
     * @param cameraRequest camera request
     */
    fun <T> openCamera(cameraView: T? = null, cameraRequest: CameraRequest? = null) {
        mCameraView = cameraView ?: mCameraView
        mCameraRequest = cameraRequest ?: getDefaultCameraRequest()
        HandlerThread("camera-${System.currentTimeMillis()}").apply {
            start()
        }.let { thread ->
            this.mCameraThread = thread
            thread
        }.also {
            mCameraHandler = Handler(it.looper, this)
            mCameraHandler?.obtainMessage(MSG_START_PREVIEW)?.sendToTarget()
        }
    }

    /**
     * Close camera
     */
    fun closeCamera() {
        mCameraHandler?.obtainMessage(MSG_STOP_PREVIEW)?.sendToTarget()
        mCameraThread?.quitSafely()
        mCameraThread = null
        mCameraHandler = null
    }

    /**
     * Get current camera request
     *
     * @return see [CameraRequest], can be null
     */
    fun getCameraRequest() = mCameraRequest

    /**
     * Update resolution
     *
     * @param width camera preview width, see [PreviewSize]
     * @param height camera preview height, [PreviewSize]
     * @return result of operation
     */
    fun updateResolution(width: Int, height: Int) {
        if (mCameraRequest == null) {
            Logger.w(TAG, "updateResolution failed, please open camera first.")
            return
        }
        mCameraRequest?.apply {
            if (previewWidth == width && previewHeight == height) {
                return@apply
            }
            Logger.i(TAG, "updateResolution: width = $width, height = $height")
            closeCamera()
            mMainHandler.postDelayed({
                previewWidth = width
                previewHeight = height
                openCamera(mCameraView, mCameraRequest)
            }, 1000)
        }
    }

    /**
     * Set camera state call back
     *
     * @param callback camera be opened or closed
     */
    fun setCameraStateCallBack(callback: ICameraStateCallBack?) {
        this.mCameraStateCallback = callback
    }

    fun getSuitableSize(maxWidth: Int, maxHeight: Int): PreviewSize {
        val sizeList = getAllPreviewSizes()
        if (sizeList.isEmpty()) {
            return PreviewSize(DEFAULT_PREVIEW_WIDTH, DEFAULT_PREVIEW_HEIGHT)
        }
        // find it
        sizeList.find {
            (it.width == maxWidth && it.height == maxHeight)
        }.also { size ->
            size ?: return@also
            return size
        }
        // find the same aspectRatio
        val aspectRatio = maxWidth.toFloat() / maxHeight
        sizeList.find {
            val w = it.width
            val h = it.height
            val ratio = w.toFloat() / h
            ratio == aspectRatio && w <= maxWidth && h <= maxHeight
        }.also { size ->
            size ?: return@also
            return size
        }
        // find the closest aspectRatio
        var minDistance: Int = maxWidth
        var closetSize = sizeList[0]
        sizeList.forEach { size ->
            if (minDistance >= abs((maxWidth - size.width))) {
                minDistance = abs(maxWidth - size.width)
                closetSize = size
            }
        }
        // use default
        sizeList.find {
            (it.width == DEFAULT_PREVIEW_WIDTH || it.height == DEFAULT_PREVIEW_HEIGHT)
        }.also { size ->
            size ?: return@also
            return size
        }
        return closetSize
    }

    fun isPreviewSizeSupported(previewSize: PreviewSize): Boolean {
        return getAllPreviewSizes().find {
            it.width == previewSize.width && it.height == previewSize.height
        } != null
    }

    private fun getDefaultCameraRequest(): CameraRequest {
        return CameraRequest.Builder()
            .setPreviewWidth(1280)
            .setPreviewHeight(720)
            .create()
    }

    companion object {
        private const val TAG = "CameraUVC"
        private const val MIN_FS = 0
        private const val MSG_START_PREVIEW = 0x01
        private const val MSG_STOP_PREVIEW = 0x02
        private const val DEFAULT_PREVIEW_WIDTH = 640
        private const val DEFAULT_PREVIEW_HEIGHT = 480

    }
}