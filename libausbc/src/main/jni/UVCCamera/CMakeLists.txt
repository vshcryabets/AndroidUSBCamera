cmake_minimum_required(VERSION 3.31)

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

    ## Force linker to include all symbols from the Sources library
    target_link_libraries(native
            PRIVATE
            libJpegStatic
            LibUvc
            android
            log
            "$<LINK_LIBRARY:WHOLE_ARCHIVE,Sources>"
            "$<LINK_LIBRARY:WHOLE_ARCHIVE,U8x8Font>"
            "$<LINK_LIBRARY:WHOLE_ARCHIVE,Images>"
    )

    # Force linker to report undefined symbols as errors during shared library creation
    target_link_options(native PRIVATE
            -Wl,--no-undefined
            -Wl,--no-allow-shlib-undefined
    )

    target_include_directories(native PUBLIC
            $<INSTALL_INTERFACE:include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )
endif()