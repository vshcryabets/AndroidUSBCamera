package com.vsh.source

class JniCallbackConsumer(
    private val callback: (frame: Frame) -> Unit
): JniConsumer() {

    override fun initNative(): Int {
        TODO("Not yet implemented")
    }

    override fun nativeRelease(ptr: Int): Int {
        TODO("Not yet implemented")
    }

    override fun nativeStopConsuming(ptr: Int): Int {
        TODO("Not yet implemented")
    }

    override fun nativeStartConsuming(ptr: Int): Int {
        TODO("Not yet implemented")
    }

    override fun consume(frame: Frame?) {
        TODO("Not yet implemented")
    }
}