package com.jiangdg.ausbc.utils

import android.content.Context
import androidx.annotation.RawRes
import java.io.BufferedReader
import java.io.IOException
import java.io.InputStreamReader

interface ReadRawTextFileUseCase {
    operator fun invoke(@RawRes id: Int): String
}

class ReadRawTextFileUseCaseImpl(private val context: Context) : ReadRawTextFileUseCase {
    override fun invoke(@RawRes id: Int): String {
        val inputStream = context.resources.openRawResource(id)
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
            Logger.e(ReadRawTextFileUseCaseImpl::class.simpleName.toString(), "open raw file failed!", e)
        }
        try {
            br.close()
        } catch (e: IOException) {
            e.printStackTrace()
            Logger.e(ReadRawTextFileUseCaseImpl::class.simpleName.toString(), "close raw file failed!", e)
        }
        return sb.toString()
    }
}