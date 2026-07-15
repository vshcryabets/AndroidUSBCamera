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

import com.jiangdg.uvc.SourceResolution
import com.vsh.LoadJniLibrary
import com.vsh.domain.usecases.GetSurfaceConsumerUseCase
import com.vsh.domain.usecases.GetTestSourceUseCase
import com.vsh.screens.testsource.TestSourceViewModel
import com.vsh.source.Frame
import com.vsh.source.JniConsumer
import com.vsh.source.JniObjectError
import com.vsh.source.JniSource
import com.vsh.source.Source
import org.junit.jupiter.api.Assertions
import org.junit.jupiter.api.BeforeAll
import org.junit.jupiter.api.Disabled
import org.junit.jupiter.api.Test

class TestSourceViewModelTests {
    val emptySource = object: JniSource<Source.OpenConfiguration, Source.ProducingConfiguration>() {
        var configuration: Source.OpenConfiguration? = null

        override fun open(configuration: Source.OpenConfiguration) {
            this.configuration = configuration
        }

        override fun initNative(): Int {
            TODO("Not yet implemented")
        }

        override fun getOpenConfiguration(): Source.OpenConfiguration = configuration!!

        override fun close() {
        }

        override fun startProducing(configuration: Source.ProducingConfiguration): JniObjectError {
            TODO("Not yet implemented")
        }

        override fun stopProducing(): JniObjectError {
            TODO("Not yet implemented")
        }

        override fun getProducingConfiguration(): Source.ProducingConfiguration? {
            TODO("Not yet implemented")
        }

        override fun isReadyForProducing(): Boolean {
            TODO("Not yet implemented")
        }

        override fun getSupportedFrameFormats(): List<Source.FrameFormat> = emptyList()

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

        override fun nativeRelease(srcId: Int) {
            TODO("Not yet implemented")
        }

        override fun nativeGetSupportedResolutions(srcId: Int): Map<Integer, List<SourceResolution>> {
            TODO("Not yet implemented")
        }

        override fun nativeGetSupportedFrameFormats(srcId: Int): List<Integer> {
            TODO("Not yet implemented")
        }

        override fun isPullSource(): Boolean = true
        override fun isPushSource(): Boolean = false
    }

    val emptyConsumer = object: JniConsumer() {
        override fun startConsuming(): JniObjectError {
            TODO("Not yet implemented")
        }

        override fun nativeRelease(ptr: Int): JniObjectError {
            TODO("Not yet implemented")
        }

        override fun nativeStopConsuming(ptr: Int): JniObjectError {
            TODO("Not yet implemented")
        }

        override fun nativeStartConsuming(ptr: Int): JniObjectError {
            TODO("Not yet implemented")
        }

        override fun consume(frame: Frame?) {
            TODO("Not yet implemented")
        }

        override fun initNative(): Int {
            return 0
        }

        override fun stopConsuming(): JniObjectError {
            TODO("Not yet implemented")
        }
    }

    val getTestSourceUseCase = object : GetTestSourceUseCase {
        override fun invoke(): JniSource<Source.OpenConfiguration, Source.ProducingConfiguration> = emptySource
    }

    val getSurfaceConsumerUseCase = object : GetSurfaceConsumerUseCase {
        override fun invoke(): JniConsumer = emptyConsumer
    }

    @Disabled
    @Test
    fun viewmodelShowResolutionsAndFps() {
        val viewModel = TestSourceViewModel(
            getTestSourceUseCase = getTestSourceUseCase,
            getSurfaceConsumerUseCase = getSurfaceConsumerUseCase
        )
        val state = viewModel.state.value
        Assertions.assertEquals(0, state.selectedResolutionIdx)
        Assertions.assertEquals(10, state.resolutionList.size)
        Assertions.assertEquals(10, state.resolutionStrs.size)
    }

    @Disabled
    @Test
    fun viewModelAllowsSelection() {
        val viewModel = TestSourceViewModel(
            getTestSourceUseCase = getTestSourceUseCase,
            getSurfaceConsumerUseCase = getSurfaceConsumerUseCase
        )
        val state = viewModel.state.value
        Assertions.assertEquals(0, state.selectedResolutionIdx)
        viewModel.onResolutionSelected(5)
        Assertions.assertEquals(5, viewModel.state.value.selectedResolutionIdx)

    }

    companion object {
        @JvmStatic
        @BeforeAll
        fun loadNativeLibrary(): Unit {
            LoadJniLibrary.loadNativeLibrary()
        }
    }
}