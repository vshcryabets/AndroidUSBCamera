package com.vsh.font

class FontSrcImpl: FontSrc {
    override fun getFontPtr(): Long = nativeGetFontPtr()

    private external fun nativeGetFontPtr(): Long
}