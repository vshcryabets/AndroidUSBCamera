package com.vsh.source

import java.util.Optional

abstract class JniConsumer : Consumer, AutoCloseable {
    protected var _srcId: Optional<Int> = Optional.empty()

    init {
        _srcId = Optional.of(initNative())
    }

    protected abstract fun initNative(): Int
    protected fun releaseNativeObject(): JniObjectError {
        if (_srcId.isEmpty)
            return JniObjectError(JniObjectErrorType.NOT_INITIALIZED)
        val result = JniObjectError.fromErrorCode(nativeRelease(_srcId.get()))
        _srcId = Optional.empty()
        return result
    }

    override fun stopConsuming(): JniObjectError {
        if (_srcId.isEmpty)
            return JniObjectError(JniObjectErrorType.NOT_INITIALIZED)
        val error = nativeStopConsuming(_srcId.get())
        return JniObjectError.fromErrorCode(error)
    }

    override fun startConsuming(): JniObjectError {
        if (_srcId.isEmpty)
            return JniObjectError(JniObjectErrorType.NOT_INITIALIZED)
        return JniObjectError.fromErrorCode(
            nativeRelease(
                nativeStartConsuming(_srcId.get())
            )
        )
    }

    fun getConsumerId(): Optional<Int> = _srcId

    override fun close() {
        stopConsuming()
        releaseNativeObject()
    }

    protected abstract fun nativeRelease(ptr: Int): Int
    protected abstract fun nativeStopConsuming(ptr: Int): Int
    protected abstract fun nativeStartConsuming(ptr: Int): Int


}