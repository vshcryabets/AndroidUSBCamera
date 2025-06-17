package com.jiangdg.usb

class UsbDeviceInfo {
    var usb_version: String = ""
    var manufacturer: String = ""
    var product: String = ""
    var version: String = ""
    var serial: String = ""

    fun clear() {
        serial = ""
        version = ""
        product = ""
        manufacturer = ""
        usb_version = ""
    }

    override fun toString(): String {
        return String.format(
            "UsbDevice:usb_version=%s,manufacturer=%s,product=%s,version=%s,serial=%s",
            usb_version,
            manufacturer,
            product,
            version,
            serial
        )
    }
}