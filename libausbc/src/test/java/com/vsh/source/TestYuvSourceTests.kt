package com.vsh.source

import com.vsh.LoadJniLibrary
import org.junit.jupiter.api.BeforeAll

class TestYuvSourceTests {

    companion object {
        @JvmStatic
        @BeforeAll
        fun loadNativeLibrary(): Unit {
            LoadJniLibrary.loadNativeLibrary()
        }
    }
}