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

import android.view.Surface
import androidx.lifecycle.ViewModel
import androidx.lifecycle.ViewModelProvider
import com.vsh.source.Source
import com.jiangdg.uvc.SourceResolution
import com.vsh.domain.usecases.GetTestSourceUseCase
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
    private val getTestSourceUseCase: GetTestSourceUseCase
) : ViewModel() {
    private val source: Source<*>
    private val _state = MutableStateFlow(TestSourceViewState())
    val state: StateFlow<TestSourceViewState> = _state

    init {
        Timber.d("TestSourceViewModel created")
        source = getTestSourceUseCase()
        source.open(
            Source.OpenConfiguration(
            tag = "TestSource"
        ))
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
    }

    fun onSurfaceReady(surface: Surface?) {

    }

    fun onSurfaceChanged(surface: Surface, format: Int, width: Int, height: Int) {
        Timber.d("Surface changed: format=$format, width=$width, height=$height")
    }
}