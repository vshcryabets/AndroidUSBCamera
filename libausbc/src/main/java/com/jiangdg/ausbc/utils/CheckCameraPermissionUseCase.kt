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
package com.jiangdg.ausbc.utils

import android.Manifest
import android.content.Context
import android.content.pm.PackageManager
import androidx.core.content.ContextCompat

interface CheckCameraPermissionUseCase {
    operator fun invoke(): Boolean
}

class CheckCameraPermissionUseCaseImpl(
    private val appContext: Context
) : CheckCameraPermissionUseCase {
    override fun invoke(): Boolean {
        val locPermission = ContextCompat.checkSelfPermission(appContext, Manifest.permission.CAMERA)
        return locPermission == PackageManager.PERMISSION_GRANTED
    }
}