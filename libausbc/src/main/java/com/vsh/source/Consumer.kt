package com.vsh.source

interface Consumer {
    fun consume(frame: Frame?)
    fun closeConsumer()
    fun openConsumer()
}