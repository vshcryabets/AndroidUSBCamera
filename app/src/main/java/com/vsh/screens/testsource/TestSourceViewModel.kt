/*
* Copyright 2025-2026 vschryabets@gmail.com
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
package com.vsh.screens.testsource

import android.view.Surface
import androidx.lifecycle.ViewModel
import androidx.lifecycle.ViewModelProvider
import com.jiangdg.uvc.SourceResolution
import com.vsh.domain.usecases.GetTestSourceUseCase
import com.vsh.source.PullToPushSource
import com.vsh.source.Source
import com.vsh.source.SurfaceConsumer
import com.vsh.source.TestSource
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.update
import timber.log.Timber

class TestSourceViewModelFactory(
    private val getTestSourceUseCase: GetTestSourceUseCase
) : ViewModelProvider.Factory {
    @Suppress("UNCHECKED_CAST")
    override fun <T : ViewModel> create(modelClass: Class<T>): T =
        TestSourceViewModel(
            getTestSourceUseCase = getTestSourceUseCase
        ) as T
}

data class TestSourceViewState(
    val selectedResolutionIdx: Int = 0,
    val resolutionStrs: List<String> = emptyList(),
    val resolutionList: List<SourceResolution> = emptyList(),
)

class TestSourceViewModel(
    getTestSourceUseCase: GetTestSourceUseCase
) : ViewModel() {
    private val source: TestSource
    private val pullToPushSource: PullToPushSource
    private val surfaceConsumer: SurfaceConsumer
    private val _state = MutableStateFlow(TestSourceViewState())
    val state: StateFlow<TestSourceViewState> = _state

    init {
        Timber.d("TestSourceViewModel created")
        source = getTestSourceUseCase()
        source.open(
            Source.OpenConfiguration(
            tag = "TestSource"
        ))
        surfaceConsumer = SurfaceConsumer()
        pullToPushSource = PullToPushSource()
        pullToPushSource.open(
            PullToPushSource.OpenConfiguration(
                tag = "PullToPushSource",
                pullSource = source,
                consumer = surfaceConsumer
            )
        )

        val sourceResolutionsMap = source.getSupportedResolutions()
        // find the first resolution with the highest FPS
        val resolutionsBySize = sourceResolutionsMap.values
            .flatten()
            .sortedBy { it.height }
        val resolutionByFps = mutableListOf<SourceResolution>()
        resolutionsBySize.forEach { size ->
            size.fps.forEach { fps ->
                resolutionByFps.add(size.copy(fps = listOf(fps)))
            }
        }
        _state.update {
            it.copy(
                resolutionList = resolutionByFps,
                resolutionStrs = resolutionByFps.map { resolutionToString(it) },
                selectedResolutionIdx = 0,
            )
        }
    }

    private fun resolutionToString(resolution: SourceResolution): String {
        return "Type ${resolution.id} - ${resolution.width}x${resolution.height} @ ${resolution.fps.firstOrNull() ?: 0}"
    }

    override fun onCleared() {
        super.onCleared()
        Timber.d("TestSourceViewModel cleared")
    }

    fun onResolutionSelected(index: Int) {
        if (index < 0 || index >= _state.value.resolutionList.size) {
            Timber.w("Invalid resolution index: $index")
            return
        }
        _state.update {
            it.copy(
                selectedResolutionIdx = index
            )
        }
    }

    fun onSurfaceDestroyed() {
        Timber.d("onSurfaceDestroyed called")
        surfaceConsumer.stopConsuming()
    }

    fun onSurfaceReady(surface: Surface) {
        if (!source.startProducing(
                Source.ProducingConfiguration(
                    tag = "TestSourceProducing",
                    width = _state.value.resolutionList[_state.value.selectedResolutionIdx].width,
                    height = _state.value.resolutionList[_state.value.selectedResolutionIdx].height,
                    fps = _state.value.resolutionList[_state.value.selectedResolutionIdx].fps.firstOrNull()
                        ?: 30f
                )
            )
                .doOnError {
                    Timber.e("Failed to start producing: ${it.type}  ${it.message}")
                }
                .isSuccess()
        ) {
            return
        }
        surfaceConsumer.setSurface(surface)
        if (!surfaceConsumer.startConsuming().doOnError {
                Timber.e("Failed to start consuming: ${it.type}  ${it.message}")
            }.isSuccess()) {
            return
        }
        pullToPushSource.startProducing(
            Source.ProducingConfiguration(
                tag = "PullToPushProducing",
                width = 0,
                height = 0,
                fps = 0f
            )
        )
    }

    fun onSurfaceChanged(surface: Surface, format: Int, width: Int, height: Int) {
        Timber.d("Surface changed: format=$format, width=$width, height=$height")
    }
}