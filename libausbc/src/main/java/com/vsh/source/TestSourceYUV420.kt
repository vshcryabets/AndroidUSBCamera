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

class TestSourceYUV420(
    private val font: FontSrc
) : JniSource<Source.OpenConfiguration, Source.ProducingConfiguration>(),
    PullSource<Source.OpenConfiguration, Source.ProducingConfiguration> {
    private var openConfiguration: Source.OpenConfiguration? = null

    override fun open(configuration: Source.OpenConfiguration) {
        super.open(configuration)
        if (_srcId.isEmpty) {
            throw IllegalStateException("Source is not initialized")
        }
        nativeOpen(_srcId.get() )
    }

    override fun initNative(): Int = nativeCreate(font.getFontPtr())

    override fun getOpenConfiguration(): Source.OpenConfiguration {
        if (openConfiguration != null)
            return openConfiguration!!
        else
            throw IllegalStateException("Source isn't initialized")
    }

    override fun close() {
        _srcId.ifPresent { nativeClose(it) }
        super.close()
    }

    override fun startProducing(configuration: Source.ProducingConfiguration) {
        _srcId.ifPresent { nativeStartProducing(it, configuration) }
    }

    override fun stopProducing() {
        _srcId.ifPresent { nativeStopProducing(it) }
    }

    override fun getProducingConfiguration(): Source.ProducingConfiguration? {
        TODO("Not yet implemented")
    }

    override fun isReadyForProducing(): Boolean {
        return _srcId.map {
            nativeIsReadyForProducing(it)
        }.orElseGet { false }
    }

    override fun readFrame(): Frame {
        var result: Frame? = null
        if (_srcId.isPresent) {
            result = nativeReadFrame(_srcId.get())
        }
        if (result == null)
            result = object : Frame {
                override fun getWidth(): Int = 0
                override fun getHeight(): Int = 0
                override fun getFormat(): Int = Source.FrameFormat.NONE.ordinal
                override fun getTimestamp(): Long = 0
            }
        return result
    }

    override fun waitNextFrame(): Boolean {
        return _srcId.map {
            nativeWaitNextFrame(it)
        }.orElseGet { false }
    }

    override fun isPullSource(): Boolean {
        TODO("Not yet implemented")
    }

    override fun isPushSource(): Boolean {
        TODO("Not yet implemented")
    }

    private external fun nativeCreate(fontPtr: Long): Int
    external override fun nativeRelease(srcId: Int)
    private external fun nativeStopProducing(srcId: Int)
    private external fun nativeClose(srcId: Int)
    private external fun nativeIsReadyForProducing(srcId: Int): Boolean
    private external fun nativeWaitNextFrame(srcId: Int): Boolean
    external override fun nativeGetSupportedResolutions(srcId: Int): Map<Integer, List<SourceResolution>>
    external override fun nativeGetSupportedFrameFormats(srcId: Int): List<Integer>
    external fun nativeOpen(srcId: Int)
    external fun nativeStartProducing(srcId: Int, configuration: Source.ProducingConfiguration)
    external fun nativeReadFrame(srcId: Int): Frame

}
