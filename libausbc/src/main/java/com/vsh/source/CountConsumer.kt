package com.vsh.source

import java.util.concurrent.atomic.AtomicInteger

class CountConsumer: JniConsumer() {

    val counter: AtomicInteger = AtomicInteger(0)

    override fun consume(frame: Frame?) {
        counter.incrementAndGet()
    }
    override fun initNative(): Int = nativeCreate()

    private external fun nativeCreate(): Int
    external override fun nativeRelease(ptr: Int)
    external override fun nativeStopConsuming(ptr: Int)
    external override fun nativeStartConsuming(ptr: Int)

    fun getFrameCount() = counter.get()
}