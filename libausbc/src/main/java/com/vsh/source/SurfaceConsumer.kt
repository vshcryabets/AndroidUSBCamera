package com.vsh.source

import android.view.Surface

class SurfaceConsumer : JniConsumer() {
    private var surface: Surface? = null

    override fun consume(frame: Frame?) {
        // No-op: frames are consumed on the native side and rendered to the configured Surface.
    }

    fun setSurface(surface: Surface, format: Int, width: Int, height: Int): JniObjectError {
        if (_srcId.isEmpty) {
            return JniObjectError(JniObjectErrorType.NOT_INITIALIZED)
        }
        this.surface = surface
        return nativeSetOpenConfiguration(
            _srcId.get(),
            surface,
            format,
            width,
            height
        )
    }

    override fun startConsuming(): JniObjectError {
        if (surface == null) {
            return JniObjectError(
                JniObjectErrorType.INVALID_ARGUMENT,
                "Surface is not set. Call setSurface() before startConsuming()"
            )
        }
        return super.startConsuming()
    }

    override fun stopConsuming(): JniObjectError {
        val result = super.stopConsuming()
        surface = null
        return result
    }

    override fun initNative(): Int = nativeCreate()

    private external fun nativeCreate(): Int
    external override fun nativeRelease(ptr: Int): JniObjectError
    external override fun nativeStopConsuming(ptr: Int): JniObjectError
    external override fun nativeStartConsuming(ptr: Int): JniObjectError
    external fun nativeSetOpenConfiguration(
        ptr: Int,
        surface: Surface,
        format: Int,
        width: Int,
        height: Int
    ): JniObjectError
}