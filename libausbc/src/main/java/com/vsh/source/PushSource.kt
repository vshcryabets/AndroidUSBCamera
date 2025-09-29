package com.vsh.source

interface PushSource<OC : PushSource.OpenConfiguration, PC : Source.ProducingConfiguration> :
    Source<OC, PC> {
    open class OpenConfiguration(
        tag: String,
        val consumer: Consumer
    ): Source.OpenConfiguration(tag) {

    }

    override fun isPullSource(): Boolean = false;
    override fun isPushSource(): Boolean = true
}