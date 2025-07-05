package com.jiangdg.uvc

/**
 * Preview format
 *
 * FORMAT_MJPEG: default format with high frame rate
 * FORMAT_YUYV: yuv format with lower frame rate
 */
enum class UvcFrameFormat(val value: Int) {
    FRAME_FORMAT_NONE(-1),
    FRAME_FORMAT_YUYV(0x0000), // Uncompressed format
    FRAME_FORMAT_MJPEG(0x0001), // MJPEG format
}