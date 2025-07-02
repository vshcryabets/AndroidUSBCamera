package com.vsh.uvc

/** VideoStreaming interface descriptor subtype (A.6) */
enum class UvcVsDeskSubtype(val value: Int) {
    UVC_VS_UNDEFINED(0x00),
    UVC_VS_FORMAT_UNCOMPRESSED(0x04),
    UVC_VS_FORMAT_MJPEG(0x06),
}