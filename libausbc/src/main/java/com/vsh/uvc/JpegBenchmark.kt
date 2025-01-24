package com.vsh.uvc

class JpegBenchmark {
    data class Results (
        val id: Int,
        val step: Int
    )

    data class Arguments (
        val image360: Int,
        val image480: Int,
        val image720: Int,
        val image1080: Int,
        val image1440: Int,
        val image2160: Int,
        val millisecondsPerStep: Int = 3000,
    )

    fun startBenchmark(args: Arguments): Int {
        return 0
    }

    fun cancelBenchmark(id: Int) {

    }

    fun getBenchmarkResults(id: Int): Results {
        return Results(
            id = id,
            step = 0
        )
    }
}