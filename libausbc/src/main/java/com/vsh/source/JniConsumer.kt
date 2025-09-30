package com.vsh.source

import java.io.Closeable

interface JniConsumer: Consumer, Closeable {
    fun getNativeObject(): Long
    override fun close()
}