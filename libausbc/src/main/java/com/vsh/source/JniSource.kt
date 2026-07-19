package com.vsh.source

import com.jiangdg.uvc.SourceResolution
import java.util.Optional

abstract class JniSource<OC : Source.OpenConfiguration, PC : ProducingConfiguration> :
    Source<OC, PC>,
    AutoCloseable {
    protected var _srcId: Optional<Int> = Optional.empty()

    override fun open(configuration: OC) {
        _srcId = Optional.of(initNative())
    }

    fun getSrcId(): Optional<Int> = _srcId

    protected abstract fun initNative(): Int

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
}