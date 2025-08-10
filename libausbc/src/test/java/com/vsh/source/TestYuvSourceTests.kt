package com.vsh.source

import com.vsh.LoadJniLibrary
import com.vsh.font.FontSrcImpl
import org.junit.jupiter.api.Assertions
import org.junit.jupiter.api.BeforeAll
import org.junit.jupiter.api.Test

class TestYuvSourceTests {

    @Test
    fun testFormatsAndResolutions() {
        val font = FontSrcImpl()
        val source = TestSourceYUV420(font)
        source.open(Source.OpenConfiguration("test"))
        val supportedFormats = source.getSupportedFrameFormats();
        val supportedResolutions = source.getSupportedResolutions();

        Assertions.assertEquals(1, supportedFormats.size)
        Assertions.assertEquals(Source.FrameFormat.YUV420P, supportedFormats[0])

        Assertions.assertEquals(1, supportedResolutions.size)
        val type = supportedResolutions.keys.first()
        val resolutions = supportedResolutions[type]!!
        Assertions.assertTrue(resolutions.size > 1)
        Assertions.assertEquals(640, resolutions[0].width)
        Assertions.assertEquals(480, resolutions[0].height)
        Assertions.assertTrue(resolutions[0].fps.size > 1)
        Assertions.assertEquals(30.0f, resolutions[0].fps[0])

        source.close();
    }

    companion object {
        @JvmStatic
        @BeforeAll
        fun loadNativeLibrary(): Unit {
            LoadJniLibrary.loadNativeLibrary()
        }
    }
}