package com.vsh.source

interface Consumer {
    fun consume(frame: Frame)
    fun stopConsuming()
}