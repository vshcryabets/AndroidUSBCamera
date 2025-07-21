package com.jiangdg.uvc

import android.view.Surface
import com.vsh.source.Source

interface DrawSourceToSurface {
    fun startDrawToSurface(
        source: Source<*>,
        surface: Surface,
        width: Int,
        height: Int,
        resolution: SourceResolution
    )

    fun stopDrawing()
}