package com.vsh.source

import com.jiangdg.uvc.SourceResolution

interface Source<OC: Source.OpenConfiguration, PC: Source.ProducingConfiguration> {

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
    open class ProducingConfiguration(
        val tag: String = "",
        val width: Int = 0,
        val height: Int = 0,
        val fps: Float = 0f,
    ) {

    }

    fun open(configuration: OC)
    fun getOpenConfiguration(): OC?
    fun close()

    fun startProducing(configuration: PC)
    fun stopProducing()
    fun getProducingConfiguration(): PC?
    fun isReadyForProducing(): Boolean
    fun getSupportedResolutions(): Map<Int, List<SourceResolution>>
    fun getSupportedFrameFormats(): List<FrameFormat>
    fun isPullSource() : Boolean
    fun isPushSource(): Boolean
}