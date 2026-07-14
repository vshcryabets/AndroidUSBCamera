package com.vsh.source

import android.view.Surface

class SurfaceConsumer: Consumer, JniConsumer() {
    private var surface: Surface? = null

    override fun consume(frame: Frame?) {
        TODO("Not yet implemented")
    }

    fun setSurface(surface: Surface) {
        this.surface = surface
    }

    override fun startConsuming(): JniObjectError {
        if (surface == null) {
            return JniObjectError(JniObjectErrorType.NOT_INITIALIZED)
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
    external override fun nativeRelease(ptr: Int): Int
    external override fun nativeStopConsuming(ptr: Int): Int
    external override fun nativeStartConsuming(ptr: Int): Int
    external fun nativeSetOpenConfiguration(ptr: Int, surface: Surface): Int
}