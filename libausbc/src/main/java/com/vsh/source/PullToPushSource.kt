package com.vsh.source

import com.jiangdg.uvc.SourceResolution

class PullToPushSource :
    JniSource<PullToPushSource.OpenConfiguration, Source.ProducingConfiguration>(),
    PushSource<PullToPushSource.OpenConfiguration, Source.ProducingConfiguration> {
    private var openConfig: OpenConfiguration? = null

    class OpenConfiguration(
        tag: String,
        val pullSource: JniSource<*, *>,
        consumer: JniConsumer
    ) : PushSource.OpenConfiguration(tag, consumer)

    override fun initNative(): Int = nativeCreate()

    override fun open(configuration: OpenConfiguration) {
        if (!configuration.pullSource.isPullSource())
            throw IllegalArgumentException(
                "Pull source expected, but got " +
                        configuration.pullSource::class.java.simpleName
            )
        super.open(configuration)
        if (_srcId.isEmpty) {
            throw IllegalStateException("Source is not initialized")
        }

        this.openConfig = configuration
        nativeOpen(
            srcId = _srcId.get(),
            tag = configuration.tag,
            pullSrcId = configuration.pullSource.getSrcId().orElseThrow(
                { IllegalStateException("Pull source is not opened") }
            ),
            consumerId = 0
        )
    }

    override fun getOpenConfiguration(): OpenConfiguration? = openConfig

    override fun startProducing(configuration: Source.ProducingConfiguration) {
        return _srcId.map {
            nativeStartProducing(it)
        }.orElseThrow({
            IllegalStateException("Source is not initialized")
        })
    }

    override fun stopProducing() {
        return _srcId.map {
            nativeStopProducing(it)
        }.orElseThrow({
            IllegalStateException("Source is not initialized")
        })
    }

    override fun getProducingConfiguration(): Source.ProducingConfiguration? {
        TODO("Not yet implemented")
    }

    override fun isReadyForProducing(): Boolean {
        TODO("Not yet implemented")
    }

    override fun isPullSource(): Boolean = false
    override fun isPushSource(): Boolean = true

    private external fun nativeCreate(): Int
    external override fun nativeRelease(srcId: Int)
    private external fun nativeOpen(srcId: Int, tag: String, pullSrcId: Int, consumerId: Int)
    external override fun nativeGetSupportedResolutions(srcId: Int): Map<Integer, List<SourceResolution>>
    external override fun nativeGetSupportedFrameFormats(srcId: Int): List<Integer>
    private external fun nativeStartProducing(srcId: Int)
    private external fun nativeStopProducing(srcId: Int)
}