package com.vsh.source

import com.jiangdg.uvc.SourceResolution

interface Source<OC: Source.OpenConfiguration> {

    enum class FrameFormat {
        YUYV,
        RGBA,
        RGB,
        RGBX,
        YUV420P,
        ENCODED,
        NONE
    }

    open class OpenConfiguration(
        val tag: String,
    ) {

    }

    fun open(configuration: OC)
    fun getOpenConfiguration(): OC?
    fun close()

    fun stopCapturing()

    fun getSupportedFrameFormats(): List<FrameFormat>
    fun getSupportedResolutions(): Map<Int, List<SourceResolution>>

    fun isPullSource() : Boolean
    fun isPushSource(): Boolean
    fun getNativeObject(): Long
}