package com.vsh.source

class CountConsumer: JniConsumer {
    private var nativePtr: Long = 0

    override fun getNativeObject(): Long = nativePtr

    override fun close() {
        if (nativePtr != 0L) {
            nativeRelease(nativePtr)
            nativePtr = 0L
        }
    }

    override fun consume(frame: Frame) {
        TODO("Not yet implemented")
    }

    override fun stopConsuming() {
        if (nativePtr != 0L) {
            nativeStopConsuming(nativePtr)
        }
    }

    protected external fun nativeCreate(): Long
    protected external fun nativeRelease(ptr: Long)
    protected external fun nativeStopConsuming(ptr: Long)
}