cmake_minimum_required(VERSION 3.22)


if(BUILD_JNIWRAPPER_MODULE)
    #message(${ANDROID_ABI})
    if (ANDROID_ABI STREQUAL "arm64-v8a")
        add_compile_definitions(ABI=1)
    elseif (ANDROID_ABI STREQUAL "armeabi-v7a")
        add_compile_definitions(ABI=2)
    elseif (ANDROID_ABI STREQUAL "x86_64")
        add_compile_definitions(ABI=3)
    elseif (ANDROID_ABI STREQUAL "x86")
        add_compile_definitions(ABI=4)
    endif ()

    add_compile_definitions(USE_TURBOJPEG)

    add_library(native SHARED
            src/_onload.cpp
            src/JniJpegBenchmark.cpp
            src/JniUVCCamera.cpp
            src/JniUVCCamera2.cpp
            src/JniTestSource.cpp
            src/LoadJpegImageFromAssetsUseCase.cpp
            src/JpegBenchmark.cpp
            src/ProgressObserver.cpp
            src/UVCCameraAdjustments.cpp
            src/UVCCaptureBase.cpp
            src/UVCPreviewJni.cpp
            src/JpegUseCases.cpp
            src/UVCCamera.cpp
            src/UVCCamera2.cpp
            src/UVCCameraJniImpl.cpp
            src/LibUvcHacks.cpp
            src/Camera.cpp
    )

    target_link_libraries(native
            Images
            Sources
            libJpegStatic
            LibUvc
            android
            log)

    target_include_directories(native PUBLIC
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )
endif()