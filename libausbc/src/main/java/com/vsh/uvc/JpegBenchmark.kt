package com.vsh.uvc

class JpegBenchmark {
    data class Results (
        val id: Int,
        val decodeCount360: Int,
        val decodeTime360Ms: Int,
        val decodeCount480: Int,
        val decodeTime480Ms: Int,
        val decodeCount720: Int,
        val decodeTime720Ms: Int,
        val decodeCount1080: Int,
        val decodeTime1080Ms: Int,
        val decodeCount1440: Int,
        val decodeTime1440Ms: Int,
        val decodeCount2160: Int,
        val decodeTime2160Ms: Int,
    )

    data class Arguments (
        val images360: Array<String>,
        val images480: Array<String>,
        val images720: Array<String>,
        val images1080: Array<String>,
        val images1440: Array<String>,
        val images2160: Array<String>,
        val decodeNumber: Int = 300,
    )

    fun startBenchmark(args: Arguments): Int = nativeStartBenchmark(args)

    fun cancelBenchmark(id: Int) {

    }

    fun getBenchmarkResults(id: Int): Results = getBenchmarkResults(id)

    private external fun nativeStartBenchmark(args: Arguments): Int
    private external fun nativeCancelBenchmark(id: Int)
    private external fun nativeGetBenchamrkResults(id: Int): Results
}