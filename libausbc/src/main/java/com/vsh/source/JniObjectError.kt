package com.vsh.source

import androidx.annotation.Keep

enum class JniObjectErrorType(val code: Int) {
    SUCCESS(0),
    UNKNOWN_ERROR(1),
    INVALID_ARGUMENT(2),
    NOT_INITIALIZED(3),
    NOT_FOUND(4),
    WRONG_CONFIG(5),
    CAPTURE_NOT_STARTED(6),
    READ_AGAIN(7),
    FRAME_NOT_READY(8)
    ;

    companion object {
        fun getById(id: Int): JniObjectErrorType =
            values().find { it.code == id } ?: INVALID_ARGUMENT
    }
}

@Keep
data class JniObjectError(
    val type: JniObjectErrorType,
    val message: String = ""
) {

    // For JNI calls, we can only pass primitive types, so we need this
    // constructor to convert the int error code to the enum type
    constructor(type: Int, message: String) : this(
        type = JniObjectErrorType.getById(type),
        message = message
    )

    fun doOnSuccess(action: () -> Unit): JniObjectError {
        if (type == JniObjectErrorType.SUCCESS) {
            action()
        }
        return this
    }

    fun doOnError(action: (JniObjectError) -> Unit): JniObjectError {
        if (type != JniObjectErrorType.SUCCESS) {
            action(this)
        }
        return this
    }

    fun isSuccess(): Boolean = type == JniObjectErrorType.SUCCESS

    companion object {
        fun fromErrorCode(errorCode: Int) = JniObjectError(
            type = JniObjectErrorType.getById(errorCode)
        )
    }
}