package com.vsh.source

import com.jiangdg.uvc.SourceResolution

interface Source<OC: Source.OpenConfiguration, PC: Source.ProducingConfiguration> {

    // TODO duplicates PixelFormat
    enum class FrameFormat(val value: Int) {
        RAW(0),
        YUYV(1),
        RGB565(2),
        RGBA(3),
        RGB(4),
        RGBX(5),
        YUV420P(6), // NV12
        NV21(7), // = YVU420SemiPlanar,NV21，但是保存到jpg颜色失真
        ENCODED(8);

        companion object {
            @JvmStatic
            fun fromValue(value: Int): FrameFormat =
                entries.firstOrNull { it.value == value } ?: RAW
        }
    }

    open class OpenConfiguration(
        val tag: String,
    ) {

    }

    // duplicates SourceResolution
    open class ProducingConfiguration(
        val tag: String = "",
        val width: Int = 0,
        val height: Int = 0,
        val fps: Float = 0f,
        val format: FrameFormat = FrameFormat.RAW
    ) {

    }

    fun open(configuration: OC)
    fun getOpenConfiguration(): OC?
    fun close()

    fun startProducing(configuration: PC): JniObjectError
    fun stopProducing(): JniObjectError
    fun getProducingConfiguration(): PC?
    fun isReadyForProducing(): Boolean
    fun getSupportedResolutions(): Map<Int, List<SourceResolution>>
    fun getSupportedFrameFormats(): List<FrameFormat>
    fun isPullSource() : Boolean
    fun isPushSource(): Boolean
}