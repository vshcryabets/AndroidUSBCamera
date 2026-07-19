package com.vsh.source

import com.jiangdg.uvc.SourceResolution

interface Source<OC: Source.OpenConfiguration, PC: ProducingConfiguration> {

    open class OpenConfiguration(
        val tag: String,
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
    fun isPullSource() : Boolean
    fun isPushSource(): Boolean
}