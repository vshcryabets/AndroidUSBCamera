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
package com.vsh.source

import com.jiangdg.uvc.SourceResolution
import com.vsh.LoadJniLibrary
import org.junit.jupiter.api.Assertions
import org.junit.jupiter.api.BeforeAll
import org.junit.jupiter.api.Disabled
import org.junit.jupiter.api.Test

class PullToPushSourceTests {

    @Disabled
    @Test
    fun pullToPushSourceAcceptsOnlyPullSource() {
        val pullSource = PullToPushSource.OpenConfiguration("pull",
            object : JniSource<Source.OpenConfiguration, Source.ProducingConfiguration>() {
            override fun open(configuration: Source.OpenConfiguration) {}
                override fun initNative(): Long = 0
                override fun getOpenConfiguration(): Source.OpenConfiguration =
                    Source.OpenConfiguration("pull")
                override fun close() {}
                override fun startProducing(configuration: Source.ProducingConfiguration) {
                    TODO("Not yet implemented")
                }
                override fun stopProducing() {}
                override fun getProducingConfiguration(): Source.ProducingConfiguration? {
                    TODO("Not yet implemented")
                }
                override fun isReadyForProducing(): Boolean = false
                override fun getSupportedFrameFormats(): List<Source.FrameFormat> = emptyList()
                override fun getSupportedResolutions(): Map<Int, List<SourceResolution>> = emptyMap()
                override fun nativeRelease(nativePtr: Long) {}
                override fun nativeGetSupportedResolutions(nativePtr: Long): Map<Integer, List<SourceResolution>> {
                    return emptyMap()
                }
                override fun nativeGetSupportedFrameFormats(nativePtr: Long): List<Integer> {
                    return emptyList()
                }
                override fun isPullSource(): Boolean = true
                override fun isPushSource(): Boolean = false
        })

        PullToPushSource().open(pullSource)

        val pushSource = PullToPushSource.OpenConfiguration("push",
            object : JniSource<Source.OpenConfiguration, Source.ProducingConfiguration>() {
                override fun initNative(): Long = 0
                override fun getOpenConfiguration(): Source.OpenConfiguration =
                    Source.OpenConfiguration("pull")
                override fun close() {}
                override fun startProducing(configuration: Source.ProducingConfiguration) {
                    TODO("Not yet implemented")
                }
                override fun stopProducing() {}
                override fun getProducingConfiguration(): Source.ProducingConfiguration? {
                    TODO("Not yet implemented")
                }
                override fun isReadyForProducing(): Boolean = false
                override fun getSupportedFrameFormats(): List<Source.FrameFormat> = emptyList()
                override fun getSupportedResolutions(): Map<Int, List<SourceResolution>> = emptyMap()
                override fun nativeRelease(nativePtr: Long) {}
                override fun nativeGetSupportedResolutions(nativePtr: Long): Map<Integer, List<SourceResolution>> {
                    return emptyMap()
                }
                override fun nativeGetSupportedFrameFormats(nativePtr: Long): List<Integer> {
                    return emptyList()
                }
                override fun isPullSource(): Boolean = false
                override fun isPushSource(): Boolean = true
            })

        try {
            PullToPushSource().open(pushSource)
            Assertions.fail("Expected IllegalArgumentException but none was thrown");
        } catch (err: IllegalArgumentException) {
        }
    }

    companion object {
        @JvmStatic
        @BeforeAll
        fun loadNativeLibrary(): Unit {
            LoadJniLibrary.loadNativeLibrary()
        }
    }

}