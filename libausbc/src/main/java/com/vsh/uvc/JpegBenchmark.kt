package com.vsh.uvc

import android.content.Context
import android.net.LocalSocket
import android.net.LocalSocketAddress
import timber.log.Timber
import java.io.BufferedReader
import java.io.File
import java.io.FileInputStream
import java.io.InputStreamReader

class JpegBenchmark(
    private val context: Context
) {
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
        val imageSamples: Map<Int, String>,
        val iterations: Int,
    )

    var ptr: Long = 0
    var socketAddr: String = ""

    init {
        val socketFile = File(context.filesDir, "jpegBenchmarkSocket")
        socketAddr = socketFile.absolutePath
        ptr = nativeCreateBenchmark(socketAddr)
    }

    fun startBenchmark(args: Arguments) {
//        val localSocketAddress = LocalSocketAddress(socketAddr, LocalSocketAddress.Namespace.FILESYSTEM)
//
//        val client = LocalSocket()
//        client.connect(localSocketAddress)
//        client.receiveBufferSize = 1024
//        client.soTimeout = 3000
//        val stream = FileInputStream(client.fileDescriptor)
//
//        Thread() {
//            val buffer = ByteArray(1024)
//            Timber.d("ASD startBenchmark thread start")
//            while (true) {
//                val readed = stream.read(buffer)
//                if (readed > 0)
//                    Timber.d("ASD readLine $readed")
//            }
//        }.start()

        nativeStartBenchmark(ptr, args)
    }

    fun cancelBenchmark(id: Int) {

    }

    fun getBenchmarkResults(id: Int): Results = getBenchmarkResults(id)

    private external fun nativeCreateBenchmark(socketFilePath: String): Long
    private external fun nativeDestroyBenchmark(ptr: Long)
    private external fun nativeGetSockFd(ptr: Long): Long

    private external fun nativeStartBenchmark(ptr: Long, args: Arguments)
    private external fun nativeCancelBenchmark(id: Long)
    private external fun nativeGetBenchamrkResults(id: Long): Results
}