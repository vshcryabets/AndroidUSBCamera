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
import org.junit.Assert
import org.junit.Test

class PullToPushSourceTests {
    @Test
    fun pullToPushSourceAcceptsOnlyPullSource() {
        val pullSource = PullToPushSource.OpenConfiguration("pull",
            object : Source<Source.OpenConfiguration> {
            override fun open(configuration: Source.OpenConfiguration) {}
            override fun getOpenConfiguration(): Source.OpenConfiguration =
                Source.OpenConfiguration("pull")
            override fun close() {}
            override fun stopCapturing() {}
            override fun getSupportedResolutions(): Map<Int, List<SourceResolution>> = emptyMap()
            override fun isPullSource(): Boolean = true
            override fun isPushSource(): Boolean = false
            override fun getNativeObject(): Long = 0L
        })

        PullToPushSource().open(pullSource)

        val pushSource = PullToPushSource.OpenConfiguration("push",
            object : Source<Source.OpenConfiguration> {
            override fun open(configuration: Source.OpenConfiguration) {}
            override fun getOpenConfiguration(): Source.OpenConfiguration =
                Source.OpenConfiguration("pull")
            override fun close() {}
            override fun stopCapturing() {}
            override fun getSupportedResolutions(): Map<Int, List<SourceResolution>> = emptyMap()
            override fun isPullSource(): Boolean = false
            override fun isPushSource(): Boolean = true
            override fun getNativeObject(): Long = 0L
            })

        try {
            PullToPushSource().open(pushSource)
            Assert.fail("Expected IllegalArgumentException but none was thrown");
        } catch (err: IllegalArgumentException) {
        }
    }

}