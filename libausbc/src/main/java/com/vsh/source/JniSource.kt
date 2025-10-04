package com.vsh.source

import com.jiangdg.uvc.SourceResolution
import java.io.Closeable
import java.util.Optional

abstract class JniSource<OC : Source.OpenConfiguration, PC : Source.ProducingConfiguration> :
    Source<OC, PC>,
    Closeable {
    protected var _srcId: Optional<Int> = Optional.empty()

    override fun open(configuration: OC) {
        _srcId = Optional.of(initNative())
    }

    fun getSrcId(): Optional<Int> = _srcId

    protected abstract fun initNative(): Int

    override fun getSupportedFrameFormats(): List<Source.FrameFormat> {
        return _srcId.map { id ->
            nativeGetSupportedFrameFormats(id).map {
                Source.FrameFormat.entries[it.toInt()]
            }
        }.orElseGet {
            emptyList()
        }
    }

    override fun close() {
        releaseNativeObject()
    }

    protected fun releaseNativeObject() {
        if (_srcId.isPresent) {
            nativeRelease(_srcId.get())
            _srcId = Optional.empty()
        }
    }

    override fun getSupportedResolutions(): Map<Int, List<SourceResolution>> {
        return _srcId.map { id ->
            nativeGetSupportedResolutions(id).mapKeys {
                it.key.toInt()
            }
        }.orElseGet {
            emptyMap()
        }
    }

    protected abstract fun nativeRelease(srcId: Int)
    protected abstract fun nativeGetSupportedResolutions(srcId: Int): Map<Integer, List<SourceResolution>>
    protected abstract fun nativeGetSupportedFrameFormats(srcId: Int): List<Integer>
}