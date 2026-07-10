package com.vsh.source

enum class JniObjectErrorType(val code: Int) {
    SUCCESS(0),
    INVALID_ARGUMENT(1),
    NOT_INITIALIZED(2),
    NOT_FOUND(3)
    ;

    companion object {
        fun getById(id: Int): JniObjectErrorType =
            values().find { it.code == id } ?: INVALID_ARGUMENT
    }
}

data class JniObjectError(
    val type: JniObjectErrorType,
    val message: String = ""
) {
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