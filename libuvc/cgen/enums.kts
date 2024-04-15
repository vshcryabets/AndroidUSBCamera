import ce.defs.*

when (target()) {
    ce.defs.Target.Kotlin, ce.defs.Target.Java -> {
        customBaseFolderPath = "../src/main/java/"
    }
    ce.defs.Target.Cxx -> {
        customBaseFolderPath = "../src/main/jni/UVCCamera/"
    }
    else -> {}
}

namespace(
    when (target()) {
        ce.defs.Target.Kotlin, ce.defs.Target.Java -> "com.vsh.uvc"
        else -> "uvc"
    }
).apply {
    enum("UvcAdjustements").apply {
        defaultType(DataType.int32)
        add("PANTILT", 1)
        // PU
        add("BRIGHTNESS")
        add("CONTRAST")
        add("HUE")
        add("SATURATION")
        add("SHARPNESS")
        add("GAMMA")
        add("WB_TEMP")
        add("WB_COMPO")
        add("BACKLIGHT")
        add("GAIN")
        add("POWERLINE_LF")
    }
}
