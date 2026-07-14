package com.vsh.source

import com.jiangdg.uvc.SourceResolution

/**
 * Sample of usage
 * 1. Prepare Consumer and Source, call Source::open()
 * 2. Create PullToPushSource.OpenConfiguration with Consumer and Source
 * 3. Open PullToPushSource with configuration
 * 4. Start source and consumer, then start producing
 * 5. Stop producing, then stop source and consumer
 * 6. Close PullToPushSource
 */
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
            consumerId = configuration.consumer.getConsumerId().orElseThrow {
                IllegalStateException("Consumer is not opened")
            }
        )
    }

    override fun getOpenConfiguration(): OpenConfiguration? = openConfig

    override fun startProducing(configuration: Source.ProducingConfiguration): JniObjectError {
        if (_srcId.isEmpty)
            return JniObjectError(JniObjectErrorType.NOT_INITIALIZED)
        val errorCode = nativeStartProducing(_srcId.get())
        return JniObjectError.fromErrorCode(errorCode)
    }

    override fun stopProducing(): JniObjectError {
        if (_srcId.isEmpty)
            return JniObjectError(JniObjectErrorType.NOT_INITIALIZED)
        val errorCode = nativeStopProducing(_srcId.get())
        return JniObjectError.fromErrorCode(errorCode)
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
    private external fun nativeStartProducing(srcId: Int): Int
    private external fun nativeStopProducing(srcId: Int): Int
}