package com.vsh.uvc

import android.content.Context
import android.os.ParcelFileDescriptor
import timber.log.Timber
import java.io.DataInputStream
import java.io.FileInputStream


class JpegBenchmark(
    private val context: Context
) {

    data class JpegBenchmarkProgress(
        var currentSampleNumber: Int = 0,
        val results: MutableList<Pair<Int, Long>> = mutableListOf(),
        var totalTime: Long = 0
    )

    private fun deserializer(reader: DataInputStream): JpegBenchmarkProgress {
        val data = JpegBenchmarkProgress()

        // Read currentSampleNumber
        data.currentSampleNumber = reader.readInt()
        Timber.d("ASD A1 ${data.currentSampleNumber}")
        // Read itemsCount
        val itemsCount = reader.readShort().toInt() // Convert from UInt16 to Int
        // Read results
        repeat(itemsCount) {
            val key = reader.readInt()
            val durationMillis = reader.readLong()
            data.results.add(key to durationMillis)
        }
        // Read totalTime
        data.totalTime = reader.readLong()
        return data
    }


    data class Arguments (
        val imageSamples: List<Pair<Int, String>>,
        val iterations: Int,
    )

    var ptr: Long = 0
    var socketAddr: String = ""

    init {
        ptr = nativeCreateBenchmark(socketAddr)
    }

    fun startBenchmark(args: Arguments) {

        val pipes = ParcelFileDescriptor.createPipe()
        val readFd = nativeStartBenchmark(ptr, args, pipes[1].fd)
        Thread() {
            Timber.d("ASD startBenchmark thread start")

            var fis = FileInputStream(pipes[0].fileDescriptor)
            var reader = DataInputStream(fis)

            while (true) {
                    val availableBytes = reader.available()

                    if (availableBytes > 5) {
                        // 1 byte type
                        // 4 bytes data size
                        val type = reader.readByte()
                        if (type == TYPE_COMPLETE || type == TYPE_DATA) {
                            val size = reader.readInt() // payload size
                            val data = deserializer(reader)
                            Timber.d("ASD Got DATA $data")
                        } else {
                            throw IllegalStateException("Unknown type $type")
                        }
                    } else {
                        Thread.sleep(10) // Wait before checking again
                    }
            }
        }.start()
    }

    fun cancelBenchmark(id: Int) {

    }

    private external fun nativeCreateBenchmark(socketFilePath: String): Long
    private external fun nativeDestroyBenchmark(ptr: Long)

    private external fun nativeStartBenchmark(ptr: Long, args: Arguments, writeFd: Int)
    private external fun nativeCancelBenchmark(id: Long)

    companion object {
        const val TYPE_COMPLETE: Byte = 1
        const val TYPE_DATA: Byte = 2
    }
}