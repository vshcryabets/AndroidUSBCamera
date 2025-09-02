package com.vsh.source

abstract class PushSource<OC : PushSource.OpenConfiguration, PC : Source.ProducingConfiguration> :
    Source<OC, PC> {
    open class OpenConfiguration(
        tag: String,
        val consumer: Consumer
    ): Source.OpenConfiguration(tag) {

    }

    private var openConfig: OC? = null
    private var consumer: Consumer? = null

    override fun open(configuration: OC) {
        this.openConfig = configuration
        this.consumer = configuration.consumer
    }

    override fun close() {
        consumer = null
        openConfig = null
    }

    open fun pushFrame(frame: Frame) {
        consumer?.consume(frame)
    }

    override fun isPullSource(): Boolean = false;
    override fun isPushSource(): Boolean = true
}