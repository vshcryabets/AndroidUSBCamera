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

    override fun closeConsumer() {
        return _srcId.map {
            nativeClose(it)
        }.orElseThrow({
            IllegalStateException("Consumer is not initialized")
        })
    }

    override fun openConsumer() {
        return _srcId.map {
            nativeOpen(it)
        }.orElseThrow({
            IllegalStateException("Consumer is not initialized")
        })
    }

    fun getConsumerId(): Optional<Int> = _srcId

    override fun close() {
        closeConsumer()
        releaseNativeObject()
    }

    protected abstract fun nativeRelease(ptr: Int)
    protected abstract fun nativeClose(ptr: Int)
    protected abstract fun nativeOpen(ptr: Int)


}