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
import android.content.pm.PackageManager
import android.os.Build
import android.os.PowerManager

/** Common Utils
 *
 * @author Created by jiangdg on 2021/12/27
 */
object Utils  {

    var debugCamera = true

    fun isTargetSdkOverP(context: Context): Boolean {
        val targetSdkVersion = try {
            val aInfo = context.packageManager.getApplicationInfo(context.packageName, 0)
            aInfo.targetSdkVersion
        } catch (e: PackageManager.NameNotFoundException) {
            return false
        }
        return targetSdkVersion >= Build.VERSION_CODES.P
    }

    fun wakeLock(context: Context): PowerManager.WakeLock {
        val pm = context.getSystemService(Context.POWER_SERVICE) as PowerManager
        val mWakeLock: PowerManager.WakeLock = pm.newWakeLock(PowerManager.SCREEN_BRIGHT_WAKE_LOCK, "jj:camera")
        mWakeLock.setReferenceCounted(false)
        mWakeLock.acquire(10*60*1000L /*10 minutes*/)
        return mWakeLock
    }

    fun wakeUnLock(wakeLock: PowerManager.WakeLock?) {
        wakeLock?.release()
    }
}