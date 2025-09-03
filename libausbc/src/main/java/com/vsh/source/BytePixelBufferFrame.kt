package com.vsh.source

import java.nio.ByteBuffer

interface BytePixelBufferFrame: Frame {
    fun getPixelBuffer(): ByteBuffer
}