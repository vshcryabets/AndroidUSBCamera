package com.jiangdg.uvc

interface IUvcCamera<OC: IUvcCamera.OpenConfiguration> {
    interface OpenConfiguration {

    }

    fun open(configuration: OC)
    fun close()
    fun stopCapturing()
}