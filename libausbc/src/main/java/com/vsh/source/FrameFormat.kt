package com.vsh.source

// TODO duplicates PixelFormat
enum class FrameFormat(val value: Int) {
    RAW(0),
    YUYV(1),
    RGB565(2),
    RGBA(3),
    RGB(4),
    RGBX(5),
    YUV420P(6), // NV12
    NV21(7), // = YVU420SemiPlanar,NV21，但是保存到jpg颜色失真
    ENCODED(8);

    companion object {
        @JvmStatic
        fun fromValue(value: Int): FrameFormat =
            entries.firstOrNull { it.value == value } ?: RAW
    }
}