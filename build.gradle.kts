// Top-level build file where you can add configuration options libuvccommon to all sub-projects/modules.
buildscript {
    repositories {
        google()
        jcenter()
    }
    dependencies {
        classpath(libs.gradle)
        classpath(libs.android.maven.gradle.plugin)
        classpath(libs.kotlin.gradle.plugin)
    }
}

plugins {
    // Existing plugins
    alias(libs.plugins.compose.compiler) apply false
}

allprojects {
    repositories {
        google()
        jcenter()
        maven { url = uri("https://jitpack.io") }
    }
}
