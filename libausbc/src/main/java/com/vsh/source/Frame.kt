package com.vsh.source

interface Frame {
    fun getWidth(): Int
    fun getHeight(): Int
    fun getFormat(): Int
    fun getTimestamp(): Long
}