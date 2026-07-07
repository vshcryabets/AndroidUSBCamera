package com.vsh.source

class JniCallbackConsumer(
    private val callback: (frame: Frame) -> Unit
): JniConsumer() {

    override fun initNative(): Int {
        TODO("Not yet implemented")
    }

    override fun nativeRelease(ptr: Int) {
        TODO("Not yet implemented")
    }

    override fun nativeClose(ptr: Int) {
        TODO("Not yet implemented")
    }

    override fun nativeOpen(ptr: Int) {
        TODO("Not yet implemented")
    }

    override fun consume(frame: Frame?) {
        TODO("Not yet implemented")
    }
}