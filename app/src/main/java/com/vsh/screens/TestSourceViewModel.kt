package com.vsh.screens

import androidx.lifecycle.ViewModel
import androidx.lifecycle.ViewModelProvider
import com.jiangdg.uvc.IUvcCamera
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
    val selectedResolutionStr: String = "",
    val selectedResolution: SourceResolution = SourceResolution.EMPTY,
    val selectedFps: Float = 0f,
    val resolutionStrs: List<String> = emptyList(),
    val resolutionList: List<SourceResolution> = emptyList(),
)

class TestSourceViewModel(
    private val getTestSourceUseCase: GetTestSourceUseCase
) : ViewModel() {
    private val source: IUvcCamera<*>
    private val _state = MutableStateFlow(TestSourceViewState())
    val state: StateFlow<TestSourceViewState> = _state

    init {
        Timber.d("TestSourceViewModel created")
        source = getTestSourceUseCase()
        source.open(IUvcCamera.OpenConfiguration(
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
        val selectedResolution = resolutionByFps.firstOrNull() ?: SourceResolution.EMPTY
        _state.update {
            it.copy(
                resolutionList = resolutionByFps,
                resolutionStrs = resolutionByFps.map { resoltuinToString(it) },
                selectedResolution = selectedResolution,
                selectedResolutionStr = resoltuinToString(selectedResolution)
            )
        }
    }

    fun resoltuinToString(resolution: SourceResolution): String {
        return "Type ${resolution.id} - ${resolution.width}x${resolution.height} @ ${resolution.fps.firstOrNull() ?: 0}"
    }

    override fun onCleared() {
        super.onCleared()
        Timber.d("TestSourceViewModel cleared")
    }
}