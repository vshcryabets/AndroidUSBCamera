package com.vsh.source

interface Frame {
    fun getWidth(): Int
    fun getHeight(): Int
    fun getFormat(): Source.FrameFormat
    fun getTimestamp(): Long
}