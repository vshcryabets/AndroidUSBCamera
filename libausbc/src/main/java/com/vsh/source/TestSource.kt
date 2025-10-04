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
import com.vsh.font.FontSrc
import java.util.Optional

class TestSource(
    private val font: FontSrc
) : JniSource<Source.OpenConfiguration, Source.ProducingConfiguration>() {
    private var openConfiguration: Source.OpenConfiguration? = null

    init {
        _srcId = Optional.of(nativeCreate(font.getFontPtr()))
    }

    override fun initNative(): Int = nativeCreate(font.getFontPtr())

    override fun open(configuration: Source.OpenConfiguration) {
    }

    override fun getOpenConfiguration(): Source.OpenConfiguration {
        if (openConfiguration != null)
            return openConfiguration!!
        else
            throw IllegalStateException("Source isn't initialized")
    }

    override fun close() {
        _srcId.ifPresent { nativeClose(it) }
        _srcId = Optional.empty()
    }

    override fun startProducing(configuration: Source.ProducingConfiguration) {
        TODO("Not yet implemented")
    }

    override fun getProducingConfiguration(): Source.ProducingConfiguration? {
        TODO("Not yet implemented")
    }

    override fun isReadyForProducing(): Boolean {
        TODO("Not yet implemented")
    }

    override fun stopProducing() {
        _srcId.ifPresent { nativeStopCapturing(it) }
    }

    override fun isPullSource(): Boolean {
        TODO("Not yet implemented")
    }

    override fun isPushSource(): Boolean {
        TODO("Not yet implemented")
    }

    private external fun nativeCreate(fontPtr: Long): Int
    external override fun nativeRelease(srcId: Int)
    private external fun nativeStopCapturing(srcId: Int)
    private external fun nativeClose(srcId: Int)
    external override fun nativeGetSupportedResolutions(srcId: Int): Map<Integer, List<SourceResolution>>
    override external fun nativeGetSupportedFrameFormats(srcId: Int): List<Integer>

}
