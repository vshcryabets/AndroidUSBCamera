package com.vsh.source

interface PullSource<OC:Source.OpenConfiguration, PC: Source.ProducingConfiguration>: Source<OC, PC> {
    fun readFrame(): Frame
    fun waitNextFrame(): Boolean
    override fun isPullSource(): Boolean = true
    override fun isPushSource(): Boolean = false
}