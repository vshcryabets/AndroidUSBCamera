package com.jiangdg.uvc

import android.view.Surface

interface DrawSourceToSurface {
    fun startDrawToSurface(
        source: TestSource,
        surface: Surface,
        width: Int,
        height: Int,
        resolution: SourceResolution
    )

    fun stopDrawing()
}