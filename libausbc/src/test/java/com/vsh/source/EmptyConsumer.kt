package com.vsh.source

import java.util.concurrent.atomic.AtomicInteger

class EmptyConsumer: JniConsumer {
    val counter: AtomicInteger = AtomicInteger(0)

    override fun consume(frame: Frame) {
        counter.incrementAndGet()
    }

    override fun stopConsuming() {

    }

    override fun getNativeObject(): Long = 0

    override fun close() {
    }
}