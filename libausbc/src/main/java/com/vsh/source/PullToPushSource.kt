package com.vsh.source

import com.jiangdg.uvc.SourceResolution

class PullToPushSource: Source<PullToPushSource.OpenConfiguration> {
    private var openConfig: OpenConfiguration? = null
    private val sourcePtr: Long

    class OpenConfiguration(
        tag: String,
        val pullSource: Source<*>
    ): Source.OpenConfiguration(tag) {
    }

    init {
        sourcePtr = nativeCreate()
    }

    override fun open(configuration: OpenConfiguration) {
        if (!configuration.pullSource.isPullSource())
            throw IllegalArgumentException("Pull source expected, but got " +
                    configuration.pullSource::class.java.simpleName
            )
        val sourcePtr = configuration.pullSource.getNativeObject()
        this.openConfig = configuration
        nativeOpen(sourcePtr, configuration.tag)
    }

    override fun getOpenConfiguration(): OpenConfiguration? = openConfig

    override fun close() {
        TODO("Not yet implemented")
    }

    override fun stopCapturing() {
        TODO("Not yet implemented")
    }

    override fun getSupportedResolutions(): Map<Int, List<SourceResolution>> {
        TODO("Not yet implemented")
    }

    override fun isPullSource(): Boolean = false
    override fun isPushSource(): Boolean = true
    override fun getNativeObject(): Long {
        TODO("Not yet implemented")
    }

    private external fun nativeCreate(): Long
    private external fun nativeRelease(ptr: Long)
    private external fun nativeOpen(sourcePtr: Long, tag: String)
}