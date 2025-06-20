/*
 * Copyright 2025 vschryabets@gmail.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.vsh.uvc

import android.os.ParcelFileDescriptor
import kotlinx.coroutines.flow.MutableSharedFlow
import kotlinx.coroutines.flow.SharedFlow
import timber.log.Timber
import java.io.BufferedInputStream
import java.io.DataInputStream
import java.io.FileInputStream

interface JpegBenchmark {
    data class JpegBenchmarkProgress(
        val currentSampleNumber: Int = 0,
        val results: MutableList<Pair<Int, Long>> = mutableListOf(),
        val totalTime: Long = 0,
        val completed: Boolean = false
    )

    data class Arguments(
        val imageSamples: List<Pair<Int, String>>,
        val iterations: Int,
    )

    fun getDecoderName(): String
    fun startBenchmark(args: Arguments): SharedFlow<JpegBenchmarkProgress>
}

class JpegBenchmarkImpl: JpegBenchmark {
    private fun deserializer(reader: DataInputStream): JpegBenchmark.JpegBenchmarkProgress {
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
        return JpegBenchmark.JpegBenchmarkProgress(
            currentSampleNumber = currentSampleNumber,
            totalTime = totalTime,
            completed = false,
            results = result
        )
    }

    var ptr: Long = 0

    init {
        ptr = nativeCreateBenchmark()
    }

    override fun startBenchmark(args: JpegBenchmark.Arguments): SharedFlow<JpegBenchmark.JpegBenchmarkProgress> {
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
        val resultsFlow = MutableSharedFlow<JpegBenchmark.JpegBenchmarkProgress>(
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

    public external override fun getDecoderName(): String
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