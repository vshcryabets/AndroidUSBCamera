package com.vsh

import java.io.File

object LoadJniLibrary {
    @JvmStatic
    fun loadNativeLibrary() {
        // Load the library by its name, without the file extension.
        // This uses an explicit path (it is not resolved by Gradle at runtime).
        val currentDir = File(System.getProperty("user.dir"))
        val rootDir = currentDir.parentFile
        val libFileName = System.mapLibraryName("JniWrapper")
        System.load(File(rootDir, "linux/build/libs/jnilib/$libFileName").absolutePath)
    }
}