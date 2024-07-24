/*
 * Copyright 2017-2023 Jiangdg
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
package com.jiangdg.ausbc.utils

import android.content.Context
import android.graphics.ImageFormat
import android.graphics.Rect
import android.graphics.YuvImage
import android.os.Build
import androidx.annotation.ChecksSdkIntAtLeast
import java.io.BufferedReader
import java.io.ByteArrayOutputStream
import java.io.File
import java.io.FileOutputStream
import java.io.IOException
import java.io.InputStreamReader

/** Media utils
 *
 * @author Created by jiangdg on 2022/2/23
 */
object MediaUtils {

    private const val TAG = "MediaUtils"

    private val AUDIO_SAMPLING_RATES = intArrayOf(
        96000,  // 0
        88200,  // 1
        64000,  // 2
        48000,  // 3
        44100,  // 4
        32000,  // 5
        24000,  // 6
        22050,  // 7
        16000,  // 8
        12000,  // 9
        11025,  // 10
        8000,  // 11
        7350,  // 12
        -1,  // 13
        -1,  // 14
        -1
    )

    fun readRawTextFile(context: Context, rawId: Int): String {
        val inputStream = context.resources.openRawResource(rawId)
        val br = BufferedReader(InputStreamReader(inputStream))
        var line: String?
        val sb = StringBuilder()
        try {
            while (br.readLine().also { line = it } != null) {
                sb.append(line)
                sb.append("\n")
            }
        } catch (e: Exception) {
            e.printStackTrace()
            Logger.e(TAG, "open raw file failed!", e)
        }
        try {
            br.close()
        } catch (e: IOException) {
            e.printStackTrace()
            Logger.e(TAG, "close raw file failed!", e)
        }
        return sb.toString()
    }

    fun saveYuv2Jpeg(path: String, data: ByteArray, width: Int, height: Int): Boolean {
        val yuvImage = try {
            YuvImage(data, ImageFormat.NV21, width, height, null)
        } catch (e: Exception) {
            Logger.e(TAG, "create YuvImage failed.", e)
            null
        } ?: return false
        val bos = ByteArrayOutputStream(data.size)
        var result = try {
            yuvImage.compressToJpeg(Rect(0, 0, width, height), 100, bos)
        } catch (e: Exception) {
            Logger.e(TAG, "compressToJpeg failed.", e)
            false
        }
        if (! result) {
            return false
        }
        val buffer = bos.toByteArray()
        val file = File(path)
        val fos: FileOutputStream?
        try {
            fos = FileOutputStream(file)
            fos.write(buffer)
            fos.close()
        } catch (e: IOException) {
            Logger.e(TAG, "saveYuv2Jpeg failed.", e)
            result = false
            e.printStackTrace()
        } finally {
            try {
                bos.close()
            } catch (e: IOException) {
                result = false
                Logger.e(TAG, "saveYuv2Jpeg failed.", e)
                e.printStackTrace()
            }
        }
        return result
    }

    @ChecksSdkIntAtLeast(api = Build.VERSION_CODES.Q)
    fun isAboveQ(): Boolean {
        return Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q
    }

}
