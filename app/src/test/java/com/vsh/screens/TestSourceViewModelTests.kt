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

import com.vsh.source.Source
import com.jiangdg.uvc.SourceResolution
import com.vsh.domain.usecases.GetTestSourceUseCase
import org.junit.jupiter.api.Assertions
import org.junit.jupiter.api.Test

class TestSourceViewModelTests {
    val emptySource = object: Source<Source.OpenConfiguration> {
        var configuration: Source.OpenConfiguration? = null

        override fun open(configuration: Source.OpenConfiguration) {
            this.configuration = configuration
        }

        override fun getOpenConfiguration(): Source.OpenConfiguration = configuration!!

        override fun close() {
        }

        override fun stopCapturing() {
        }

        override fun getSupportedResolutions(): Map<Int, List<SourceResolution>> {
            return mapOf(
                0 to listOf(
                    SourceResolution(0, 640, 480, listOf(15.0f,30.0f,60.0f)),
                    SourceResolution(0, 1280, 720, listOf(30.0f)),
                    SourceResolution(0, 1920, 1080, listOf(30.0f))
                ),
                1 to listOf(
                    SourceResolution(1, 640, 480, listOf(15.0f,30.0f,60.0f)),
                    SourceResolution(1, 1280, 720, listOf(30.0f)),
                    SourceResolution(1, 1920, 1080, listOf(30.0f))
                ),
            )
        }

        override fun isPullSource(): Boolean = true
        override fun isPushSource(): Boolean = false
        override fun getNativeObject(): Long {
            TODO("Not yet implemented")
        }
    }

    val getTestSourceUseCase = object : GetTestSourceUseCase {
        override fun invoke(): Source<Source.OpenConfiguration> = emptySource
    }

    @Test
    fun viewmodelShowResolutionsAndFps() {
        val viewModel = TestSourceViewModel(
            getTestSourceUseCase = getTestSourceUseCase
        )
        val state = viewModel.state.value
        Assertions.assertEquals(0, state.selectedResolutionIdx)
        Assertions.assertEquals(10, state.resolutionList.size)
        Assertions.assertEquals(10, state.resolutionStrs.size)
    }

    @Test
    fun viewModelAllowsSelection() {
        val viewModel = TestSourceViewModel(
            getTestSourceUseCase = getTestSourceUseCase
        )
        val state = viewModel.state.value
        Assertions.assertEquals(0, state.selectedResolutionIdx)
        viewModel.onResolutionSelected(5)
        Assertions.assertEquals(5, viewModel.state.value.selectedResolutionIdx)

    }
}