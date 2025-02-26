package com.jiangdg.ausbc

import android.content.Context
import android.graphics.SurfaceTexture
import android.hardware.usb.UsbDevice
import android.os.Environment
import android.os.Handler
import android.os.HandlerThread
import android.os.Looper
import android.os.Message
import com.jiangdg.ausbc.callback.ICameraStateCallBack
import com.jiangdg.ausbc.callback.IDeviceConnectCallBack
import com.jiangdg.ausbc.callback.IPreviewDataCallBack
import com.jiangdg.ausbc.camera.bean.CameraRequest
import com.jiangdg.ausbc.camera.bean.PreviewSize
import com.jiangdg.ausbc.render.RenderManager
import com.jiangdg.ausbc.render.env.RotateType
import com.jiangdg.ausbc.utils.CameraUtils.isFilterDevice
import com.jiangdg.ausbc.utils.CameraUtils.isUsbCamera
import com.jiangdg.ausbc.utils.Logger
import com.jiangdg.ausbc.utils.OpenGLUtils
import com.jiangdg.ausbc.utils.SettableFuture
import com.jiangdg.ausbc.utils.Utils
import com.jiangdg.ausbc.widget.IAspectRatio
import com.jiangdg.usb.USBMonitor
import java.text.SimpleDateFormat
import java.util.Locale
import java.util.concurrent.CopyOnWriteArrayList
import java.util.concurrent.TimeUnit
import kotlin.math.abs

/** Multi-road camera client
 *
 * @author Created by jiangdg on 2022/7/18
 *      Modified for v3.3.0 by jiangdg on 2023/1/15
 */
class MultiCameraClient(ctx: Context, callback: IDeviceConnectCallBack?) {
    private var mUsbMonitor: USBMonitor? = null
    private val mMainHandler by lazy {
        Handler(Looper.getMainLooper())
    }

    init {
        mUsbMonitor = USBMonitor(ctx, object : USBMonitor.OnDeviceConnectListener {
            /**
             * Called by receive usb device pulled out broadcast
             *
             * @param device usb device info,see [UsbDevice]
             */
            override fun onDetach(device: UsbDevice?) {
                if (Utils.debugCamera) {
                    Logger.i(TAG, "detach device name/pid/vid:${device?.deviceName}&${device?.productId}&${device?.vendorId} ")
                }
                device ?: return
                if (!isUsbCamera(device) && !isFilterDevice(ctx, device)) {
                    return
                }
                mMainHandler.post {
                    callback?.onDetachDec(device)
                }
            }

            /**
             * Called by granted permission
             *
             * @param device usb device info,see [UsbDevice]
             */
            override fun onConnect(
                device: UsbDevice?,
                ctrlBlock: USBMonitor.UsbControlBlock?,
                createNew: Boolean
            ) {
                if (Utils.debugCamera) {
                    Logger.i(TAG, "connect device name/pid/vid:${device?.deviceName}&${device?.productId}&${device?.vendorId} ")
                }
                device ?: return
                if (!isUsbCamera(device) && !isFilterDevice(ctx, device)) {
                    return
                }
                mMainHandler.post {
                    callback?.onConnectDev(device, ctrlBlock)
                }
            }

            /**
             * Called by dis unauthorized permission
             *
             * @param device usb device info,see [UsbDevice]
             */
            override fun onDisconnect(device: UsbDevice?, ctrlBlock: USBMonitor.UsbControlBlock?) {
                if (Utils.debugCamera) {
                    Logger.i(TAG, "disconnect device name/pid/vid:${device?.deviceName}&${device?.productId}&${device?.vendorId} ")
                }
                device ?: return
                if (!isUsbCamera(device) && !isFilterDevice(ctx, device)) {
                    return
                }
                mMainHandler.post {
                    callback?.onDisConnectDec(device, ctrlBlock)
                }
            }


            /**
             * Called by dis unauthorized permission or request permission exception
             *
             * @param device usb device info,see [UsbDevice]
             */
            override fun onCancel(device: UsbDevice?) {
                if (Utils.debugCamera) {
                    Logger.i(TAG, "cancel device name/pid/vid:${device?.deviceName}&${device?.productId}&${device?.vendorId} ")
                }
                device ?: return
                if (!isUsbCamera(device) && !isFilterDevice(ctx, device)) {
                    return
                }
                mMainHandler.post {
                    callback?.onCancelDev(device)
                }
            }
        })
    }

    /**
     * Register usb insert broadcast
     */
    fun register() {
        if (isMonitorRegistered()) {
            return
        }
        if (Utils.debugCamera) {
            Logger.i(TAG, "register...")
        }
        mUsbMonitor?.register()
    }

    /**
     * UnRegister usb insert broadcast
     */
    fun unRegister() {
        if (!isMonitorRegistered()) {
            return
        }
        if (Utils.debugCamera) {
            Logger.i(TAG, "unRegister...")
        }
        mUsbMonitor?.unregister()
    }

    /**
     * Request usb device permission
     *
     * @param device see [UsbDevice]
     * @return true ready to request permission
     */
    fun requestPermission(device: UsbDevice?): Boolean {
        if (!isMonitorRegistered()) {
            Logger.w(TAG, "Usb monitor haven't been registered.")
            return false
        }
        mUsbMonitor?.requestPermission(device)
        return true
    }

    /**
     * Destroy usb monitor engine
     */
    fun destroy() {
        mUsbMonitor?.destroy()
    }

    private fun isMonitorRegistered() = mUsbMonitor?.isRegistered == true


    /**
     * Camera strategy super class
     *
     * @property ctx context
     * @property device see [UsbDevice]
     * @constructor Create camera by inherit it
     */
    abstract class ICamera(val ctx: Context, val device: UsbDevice): Handler.Callback {
        private var mCameraThread: HandlerThread? = null
        private var mRenderManager: RenderManager?  = null
        private var mCameraView: Any? = null
        private var mCameraStateCallback: ICameraStateCallBack? = null
        private var mSizeChangedFuture: SettableFuture<Pair<Int, Int>>? = null
        protected var mCameraRequest: CameraRequest? = null
        protected var mCameraHandler: Handler? = null
        protected var isPreviewed: Boolean = false
        protected var isNeedGLESRender: Boolean = false
        protected var mCtrlBlock: USBMonitor.UsbControlBlock? = null
        protected var mPreviewDataCbList = CopyOnWriteArrayList<IPreviewDataCallBack>()
        protected val mMainHandler: Handler by lazy {
            Handler(Looper.getMainLooper())
        }
        protected val mDateFormat by lazy {
            SimpleDateFormat("yyyyMMddHHmmssSSS", Locale.getDefault())
        }
        protected val mCameraDir by lazy {
            "${Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DCIM)}/Camera"
        }

        override fun handleMessage(msg: Message): Boolean {
            when (msg.what) {
                MSG_START_PREVIEW -> {
                    val previewWidth = mCameraRequest!!.previewWidth
                    val previewHeight = mCameraRequest!!.previewHeight
                    val renderMode = mCameraRequest!!.renderMode
                    val isRawPreviewData = mCameraRequest!!.isRawPreviewData
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
                    }.also { view->
                        isNeedGLESRender = isGLESRender(renderMode == CameraRequest.RenderMode.OPENGL)
                        if (! isNeedGLESRender && view != null) {
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
                        val previewCb = if (isRawPreviewData) {
                            null
                        } else {
                            mPreviewDataCbList
                        }
                        mRenderManager = RenderManager(ctx, previewWidth, previewHeight, previewCb)
                        mRenderManager?.startRenderScreen(screenWidth, screenHeight, surface, object : RenderManager.CameraSurfaceTextureListener {
                            override fun onSurfaceTextureAvailable(surfaceTexture: SurfaceTexture?) {
                                if (surfaceTexture == null) {
                                    closeCamera()
                                    postStateEvent(ICameraStateCallBack.State.ERROR, "create camera surface failed")
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

        protected abstract fun <T> openCameraInternal(cameraView: T)
        protected abstract fun closeCameraInternal()

        /**
         * should use opengl, recommend
         *
         * @return default depend on device opengl version, >=2.0 is true
         */
        private fun isGLESRender(isGlesRenderOpen: Boolean): Boolean =isGlesRenderOpen && OpenGLUtils.isGlEsSupported(ctx)

        /**
         * Put video data
         *
         * @param data NV21 raw data
         */
        protected fun putVideoData(data: ByteArray) {
        }

        /**
         * Rotate camera render angle
         *
         * @param type rotate angle, null means rotating nothing
         * see [RotateType.ANGLE_90], [RotateType.ANGLE_270],...etc.
         */
        fun setRotateType(type: RotateType?) {
            mRenderManager?.setRotateType(type)
        }

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
        fun setUsbControlBlock(ctrlBlock: USBMonitor.UsbControlBlock?) {
            this.mCtrlBlock = ctrlBlock
        }

        /**
         * Get all preview sizes
         *
         * @param aspectRatio aspect ratio
         * @return [PreviewSize] list of camera
         */
        abstract fun getAllPreviewSizes(aspectRatio: Double? = null): MutableList<PreviewSize>

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
         * check if camera opened
         *
         * @return camera open status, true or false
         */
        fun isCameraOpened() = isPreviewed

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

        /**
         * Add preview data call back
         * @param callBack preview data call back
         */
        fun addPreviewDataCallBack(callBack: IPreviewDataCallBack) {
            if (mPreviewDataCbList.contains(callBack)) {
                return
            }
            mPreviewDataCbList.add(callBack)
        }

        /**
         * Remove preview data call back
         *
         * @param callBack preview data call back
         */
        fun removePreviewDataCallBack(callBack: IPreviewDataCallBack) {
            if (! mPreviewDataCbList.contains(callBack)) {
                return
            }
            mPreviewDataCbList.remove(callBack)
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
    }

    companion object {
        private const val TAG = "MultiCameraClient"
        private const val MSG_START_PREVIEW = 0x01
        private const val MSG_STOP_PREVIEW = 0x02
        private const val DEFAULT_PREVIEW_WIDTH = 640
        private const val DEFAULT_PREVIEW_HEIGHT = 480
        const val MAX_NV21_DATA = 5
        const val CAPTURE_TIMES_OUT_SEC = 3L
    }
}