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

class TestSource(
    private val font: FontSrc
) : JniSource<Source.OpenConfiguration, Source.ProducingConfiguration>() {
    private var openConfiguration: Source.OpenConfiguration? = null

    init {
        nativePtr = nativeCreate(font.getFontPtr())
    }

    override fun initNative(): Long = nativeCreate(font.getFontPtr())

    override fun open(configuration: Source.OpenConfiguration) {
    }

    override fun getOpenConfiguration(): Source.OpenConfiguration {
        if (openConfiguration != null)
            return openConfiguration!!
        else
            throw IllegalStateException("Source isn't initialized")
    }

    override fun close() {
        if (nativePtr != 0L) {
            nativeClose(nativePtr)
        }
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
        if (nativePtr != 0L) {
            nativeStopCapturing(nativePtr)
        }
    }

    override fun isPullSource(): Boolean {
        TODO("Not yet implemented")
    }

    override fun isPushSource(): Boolean {
        TODO("Not yet implemented")
    }

    private external fun nativeCreate(fontPtr: Long): Long
    external override fun nativeRelease(ptr: Long)
    private external fun nativeStopCapturing(ptr: Long)
    private external fun nativeClose(ptr: Long)
    external override fun nativeGetSupportedResolutions(ptr: Long): Map<Integer, List<SourceResolution>>
    override external fun nativeGetSupportedFrameFormats(nativePtr: Long): List<Integer>

}
