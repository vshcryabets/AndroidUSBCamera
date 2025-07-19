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
package com.jiangdg.uvc

class TestSource : IUvcCamera<IUvcCamera.OpenConfiguration> {
    private var openConfiguration: IUvcCamera.OpenConfiguration? = null
    private var nativePtr : Long = 0L;

    init {
        System.loadLibrary("native")
        nativePtr = nativeCreate()
    }

    override fun open(configuration: IUvcCamera.OpenConfiguration) {
    }

    override fun getOpenConfiguration(): IUvcCamera.OpenConfiguration {
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

    override fun stopCapturing() {
        if (nativePtr != 0L) {
            nativeStopCapturing(nativePtr)
        }
    }

    override fun getSupportedResolutions(): Map<Int, List<SourceResolution>> {
        if (nativePtr != 0L) {
            return nativeGetSupportedResolutions(nativePtr).mapKeys {
                it.key.toInt()
            }
        } else {
            return emptyMap()
        }
    }

    private external fun nativeCreate(): Long
    private external fun nativeRelease(ptr: Long)
    private external fun nativeStopCapturing(ptr: Long)
    private external fun nativeClose(ptr: Long)
    private external fun nativeGetSupportedResolutions(ptr: Long): Map<Integer, List<SourceResolution>>

}
