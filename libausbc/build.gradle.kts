plugins {
    id("com.android.library")
    id("kotlin-android")
}

android {
    compileSdk = libs.versions.compileSdk.get().toInt()
    namespace = "com.jiangdg.ausbc"
    ndkVersion = libs.versions.ndk.get()

    defaultConfig {
        minSdk = libs.versions.minSdk.get().toInt()
        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
        ndk.abiFilters.addAll(listOf("armeabi-v7a","arm64-v8a")) // x86, x86_64 still in progress
        externalNativeBuild {
            cmake {
                arguments.add("-DANDROID_ALIGNED_AS_PAGE_SIZE=ON")
                cppFlags += "-Wl,-z,max-page-size=16384"
            }
        }
    }

    buildTypes {
        release {
            isMinifyEnabled = false
            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt"),
                "proguard-rules.pro"
            )
        }
    }
    compileOptions {
        sourceCompatibility(libs.versions.jvmTarget.get())
        targetCompatibility(libs.versions.jvmTarget.get())
    }
    kotlinOptions {
        jvmTarget = libs.versions.jvmTarget.get()
    }
    externalNativeBuild {
        cmake.version = libs.versions.cmake.get()
        cmake.path = File(projectDir, "CMakeLists.txt")
    }
    testOptions {
        targetSdk = libs.versions.targetSdk.get().toInt()
        unitTests.all {
            it.useJUnitPlatform()
        }
    }
    lint {
        targetSdk = libs.versions.targetSdk.get().toInt()
    }
}

dependencies {
    implementation(libs.androidx.appcompat)
    implementation(libs.androidx.constraintlayout)
    implementation(libs.material)
    implementation(libs.timber)
    testImplementation(libs.junit.jupiter.api)
    testRuntimeOnly(libs.junit.jupiter.engine)
}