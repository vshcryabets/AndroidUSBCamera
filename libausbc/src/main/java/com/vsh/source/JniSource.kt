package com.vsh.source

import com.jiangdg.uvc.SourceResolution

abstract class JniSource<OC : Source.OpenConfiguration>: Source<OC> {
    protected var nativePtr : Long = 0L

    override fun open(configuration: OC) {
        nativePtr = initNative()
    }

    override fun getNativeObject(): Long = nativePtr

    protected abstract fun initNative(): Long

    override fun getSupportedFrameFormats(): List<Source.FrameFormat> {
        if (nativePtr != 0L) {
            return nativeGetSupportedFrameFormats(nativePtr).map {
                Source.FrameFormat.entries[it.toInt()]
            }
        } else {
            return emptyList()
        }
    }

    override fun getSupportedResolutions(): Map<Int, List<SourceResolution>> {
        if (nativePtr != 0L) {
            return nativeGetSupportedResolutions(nativePtr).mapKeys {
                it.key.toInt()
            }
        } else {
            return emptyMap()
        }
    }

    abstract fun nativeGetSupportedResolutions(nativePtr: Long): Map<Integer, List<SourceResolution>>
    abstract fun nativeGetSupportedFrameFormats(nativePtr: Long): List<Integer>
}