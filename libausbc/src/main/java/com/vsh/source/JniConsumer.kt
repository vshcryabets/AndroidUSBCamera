package com.vsh.source

import java.util.Optional

abstract class JniConsumer: Consumer, AutoCloseable {
    protected var _srcId: Optional<Int> = Optional.empty()

    init {
        _srcId = Optional.of(initNative())
    }

    protected abstract fun initNative(): Int
    protected fun releaseNativeObject() {
        if (_srcId.isPresent) {
            nativeRelease(_srcId.get())
            _srcId = Optional.empty()
        }
    }

    override fun stopConsuming() {
        return _srcId.map {
            nativeStopConsuming(it)
        }.orElseThrow({
            IllegalStateException("Consumer is not initialized")
        })
    }

    fun getConsumerId(): Optional<Int> = _srcId

    override fun close() {
        stopConsuming()
        releaseNativeObject()
    }

    protected abstract fun nativeRelease(ptr: Int)
    protected abstract fun nativeStopConsuming(ptr: Int)


}