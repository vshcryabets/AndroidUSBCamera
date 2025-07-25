package com.vsh.source

import com.jiangdg.uvc.SourceResolution

interface Source<OC: Source.OpenConfiguration> {
    open class OpenConfiguration(
        public val tag: String,
    ) {

    }

    fun open(configuration: OC)
    fun getOpenConfiguration(): OC?
    fun close()

    fun stopCapturing()

    fun getSupportedResolutions(): Map<Int, List<SourceResolution>>

    fun isPullSource() : Boolean
    fun isPushSource(): Boolean
    fun getNativeObject(): Long
}