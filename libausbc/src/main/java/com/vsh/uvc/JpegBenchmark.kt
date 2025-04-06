package com.vsh.uvc

import android.content.Context
import android.os.ParcelFileDescriptor
import kotlinx.coroutines.flow.MutableSharedFlow
import kotlinx.coroutines.flow.SharedFlow
import timber.log.Timber
import java.io.BufferedInputStream
import java.io.DataInputStream
import java.io.FileInputStream


class JpegBenchmark(
    private val context: Context
) {

    data class JpegBenchmarkProgress(
        val currentSampleNumber: Int = 0,
        val results: MutableList<Pair<Int, Long>> = mutableListOf(),
        val totalTime: Long = 0,
        val completed: Boolean = false
    )

    private fun deserializer(reader: DataInputStream): JpegBenchmarkProgress {
        // Read currentSampleNumber
        val currentSampleNumber = reader.readInt()
        // Read itemsCount
        val itemsCount = reader.readShort().toInt() // Convert from UInt16 to Int
        // Read results
        val result = mutableListOf<Pair<Int, Long>>()
        repeat(itemsCount) {
            val key = reader.readInt()
            val durationMillis = reader.readLong()
            result.add(key to durationMillis)
        }
        // Read totalTime
        val totalTime = reader.readLong()
        return JpegBenchmarkProgress(
            currentSampleNumber = currentSampleNumber,
            totalTime = totalTime,
            completed = false,
            results = result
        )
    }

    data class Arguments(
        val imageSamples: List<Pair<Int, String>>,
        val iterations: Int,
    )

    var ptr: Long = 0

    init {
        ptr = nativeCreateBenchmark()
    }

    fun startBenchmark(args: Arguments): SharedFlow<JpegBenchmarkProgress> {
        val pipes = ParcelFileDescriptor.createPipe()
        val sampleIds = args.imageSamples.map { it.first }.toIntArray()
        val sampelStrings = args.imageSamples.map { it.second }.toTypedArray()
        nativeStartBenchmark(
            ptr,
            iterations = args.iterations,
            sampleIds = sampleIds,
            sampleStrings = sampelStrings,
            writeFd = pipes[1].fd
        )
        val resultsFlow = MutableSharedFlow<JpegBenchmarkProgress>(
            replay = 0,
            extraBufferCapacity = 1
        )

        Thread {
            Timber.d("startBenchmark thread start")
            val fis = FileInputStream(pipes[0].fileDescriptor)
            val reader = DataInputStream(BufferedInputStream(fis))

            while (true) {
                if (reader.available() > 5) {
                    // 1 byte type
                    // 4 bytes data size
                    val type = reader.readByte()
                    if (type == TYPE_COMPLETE || type == TYPE_DATA) {
                        val size = reader.readInt() // payload size
                        var data = deserializer(reader)
                        val complete = type == TYPE_COMPLETE
                        if (complete) {
                            data = data.copy(completed = true)
                        }
                        val emitResult = resultsFlow.tryEmit(data)
                        if (complete) {
                            break
                        }
                    } else {
                        throw IllegalStateException("Unknown type $type")
                    }
                } else {
                    Thread.sleep(10) // Reduce sleep time to check more frequently
                }
            }
            Timber.d("startBenchmark thread end")
        }.start()
        return resultsFlow
    }

    fun cancelBenchmark(id: Int) {

    }

    public external fun getDecoderName(): String
    private external fun nativeCreateBenchmark(): Long
    private external fun nativeDestroyBenchmark(ptr: Long)

    private external fun nativeStartBenchmark(
        ptr: Long, iterations: Int,
        sampleIds: IntArray,
        sampleStrings: Array<String>,
        writeFd: Int
    )

    private external fun nativeCancelBenchmark(id: Long)

    companion object {
        const val TYPE_COMPLETE: Byte = 2
        const val TYPE_DATA: Byte = 1
    }
}