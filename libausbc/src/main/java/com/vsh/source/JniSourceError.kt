package com.vsh.source

enum class JniSourceErrorType(val code: Int) {
    SUCCESS(0),
    INVALID_ARGUMENT(1),
    SOURCE_NOT_INITIALIZED(2),
    SOURCE_NOT_FOUND(3)
    ;

    companion object {
        fun getById(id: Int): JniSourceErrorType =
            values().find { it.code == id } ?: INVALID_ARGUMENT
    }
}

data class JniSourceError(
    val type: JniSourceErrorType,
    val message: String = ""
) {
    fun doOnSuccess(action: () -> Unit): JniSourceError {
        if (type == JniSourceErrorType.SUCCESS) {
            action()
        }
        return this
    }

    fun doOnError(action: (JniSourceError) -> Unit): JniSourceError {
        if (type != JniSourceErrorType.SUCCESS) {
            action(this)
        }
        return this
    }

    fun isSuccess(): Boolean = type == JniSourceErrorType.SUCCESS

    companion object {
        fun fromErrorCode(errorCode: Int) = JniSourceError(
            type = JniSourceErrorType.getById(errorCode)
        )
    }
}