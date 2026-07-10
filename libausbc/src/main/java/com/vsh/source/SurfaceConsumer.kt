package com.vsh.source

import android.view.Surface

class SurfaceConsumer: Consumer, JniConsumer() {
    override fun consume(frame: Frame?) {
        TODO("Not yet implemented")
    }

    override fun stopConsuming(): JniObjectError {
        TODO("Not yet implemented")
    }

    override fun startConsuming(): JniObjectError {
        TODO("Not yet implemented")
    }

    override fun initNative(): Int = nativeCreate()

    private external fun nativeCreate(): Int
    external override fun nativeRelease(ptr: Int): Int
    external override fun nativeStopConsuming(ptr: Int): Int
    external override fun nativeStartConsuming(ptr: Int): Int
    external fun nativeSetOpenConfiguration(ptr: Int, surface: Surface): Int
}