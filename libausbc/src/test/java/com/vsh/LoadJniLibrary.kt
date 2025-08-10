package com.vsh

import java.io.File

object LoadJniLibrary {
    @JvmStatic
    fun loadNativeLibrary() {
        // Load the library by its name, without the file extension.
        // Gradle will find "my_native_lib.dll" in the root folder.
        val currentDir = File(System.getProperty("user.dir"))
        val rootDir = currentDir.parentFile
        val libFileName = System.mapLibraryName("JniWrapper")
        System.load( File(rootDir, "linux/build/libs/jnilib/$libFileName").absolutePath)

    }
}