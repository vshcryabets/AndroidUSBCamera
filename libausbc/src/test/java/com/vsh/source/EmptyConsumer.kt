package com.vsh.source

import java.util.concurrent.atomic.AtomicInteger

class EmptyConsumer: Consumer {
    val counter: AtomicInteger = AtomicInteger(0)

    override fun consume(frame: Frame) {
        counter.incrementAndGet()
    }

    override fun stopConsuming() {

    }
}