package com.jiangdg.uvc

interface IUvcCamera<OC: IUvcCamera.OpenConfiguration> {
    open class OpenConfiguration(
        public val tag: String,
    ) {

    }

    fun open(configuration: OC)
    fun getOpenConfiguration(): OC
    fun close()

    fun stopCapturing()

    fun getSupportedResolutions(): Map<Int, List<SourceResolution>>
}