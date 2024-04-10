import ce.defs.*

namespace(
    when (target()) {
        ce.defs.Target.Kotlin, ce.defs.Target.Java -> "com.vsh.uvc"
        else -> "uvc"
    }
).apply {
    enum("UvcAjustements").apply {
        add("PAN", 1)
        add("TILT")
        add("BRIGHTNESS")
    }
}
