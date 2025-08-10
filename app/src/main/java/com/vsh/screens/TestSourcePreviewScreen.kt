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
package com.vsh.screens

import android.view.SurfaceHolder
import android.view.SurfaceView
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import androidx.compose.ui.viewinterop.AndroidView
import androidx.navigation.NavController
import timber.log.Timber

@Composable
fun TestSourcePreview(
    navController: NavController,
    viewModel: TestSourceViewModel
) {

    val uiState by viewModel.state.collectAsState()
    Column {
        Text(
            "Preview",
            modifier = Modifier.padding(16.dp)
        )
        AndroidView(
            modifier = Modifier.fillMaxSize(),
            factory = { context ->
                SurfaceView(context).apply {
                     holder.addCallback(object : SurfaceHolder.Callback {
                         override fun surfaceCreated(holder: SurfaceHolder) {
                             viewModel.onSurfaceReady(holder.surface)
                         }

                         override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {
                             viewModel.onSurfaceChanged(holder.surface!!, format, width, height)
                         }

                         override fun surfaceDestroyed(holder: SurfaceHolder) {
                             viewModel.onSurfaceDestroyed()
                         }
                     })
                }
            },
        )
    }
}

