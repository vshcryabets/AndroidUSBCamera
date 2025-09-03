package com.vsh.source

import java.nio.ByteBuffer

class BytePixelBufferFrame(
    private val width: Int,
    private val height: Int,
    private val format: Int,
    private val timestamp: Long,
    private val pixelBuffer: ByteBuffer
): Frame {
    override fun getWidth(): Int = width
    override fun getHeight(): Int = height
    override fun getFormat(): Int = format
    override fun getTimestamp(): Long = timestamp
    fun getPixelBuffer(): ByteBuffer = pixelBuffer
}