package com.vsh.source

import com.jiangdg.uvc.SourceResolution

class PullToPushSource :
    JniSource<PullToPushSource.OpenConfiguration, Source.ProducingConfiguration>(),
    PushSource<PullToPushSource.OpenConfiguration, Source.ProducingConfiguration> {
    private var openConfig: OpenConfiguration? = null

    class OpenConfiguration(
        tag: String,
        val pullSource: JniSource<*, *>,
        consumer: Consumer
    ) : PushSource.OpenConfiguration(tag, consumer)

    override fun initNative(): Long = nativeCreate()

    override fun open(configuration: OpenConfiguration) {
        if (!configuration.pullSource.isPullSource())
            throw IllegalArgumentException(
                "Pull source expected, but got " +
                        configuration.pullSource::class.java.simpleName
            )
        super.open(configuration)
        val sourcePtr = configuration.pullSource.getNativeObject()
        this.openConfig = configuration
        nativeOpen(sourcePtr, configuration.tag)
    }

    override fun getOpenConfiguration(): OpenConfiguration? = openConfig

    override fun startProducing(configuration: Source.ProducingConfiguration) {
        TODO("Not yet implemented")
    }

    override fun stopProducing() {
        TODO("Not yet implemented")
    }

    override fun getProducingConfiguration(): Source.ProducingConfiguration? {
        TODO("Not yet implemented")
    }

    override fun isReadyForProducing(): Boolean {
        TODO("Not yet implemented")
    }

    override fun isPullSource(): Boolean = false
    override fun isPushSource(): Boolean = true

    private external fun nativeCreate(): Long
    external override fun nativeRelease(ptr: Long)
    private external fun nativeOpen(sourcePtr: Long, tag: String)
    external override fun nativeGetSupportedResolutions(ptr: Long): Map<Integer, List<SourceResolution>>
    external override fun nativeGetSupportedFrameFormats(ptr: Long): List<Integer>
}