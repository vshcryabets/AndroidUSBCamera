cmake_minimum_required(VERSION 3.22)

set(UVCCAMERA ${CMAKE_CURRENT_LIST_DIR}/)
include_directories(
    ${UVCCAMERA}/include/
)


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

list(APPEND SOURCES
	${UVCCAMERA}/src/_onload.cpp
    ${UVCCAMERA}/src/UVCCamera.cpp
    ${UVCCAMERA}/src/ImageUseCases.cpp
    ${UVCCAMERA}/src/UVCPreviewBase.cpp
    ${UVCCAMERA}/src/UVCPreviewJni.cpp
    ${UVCCAMERA}/src/UVCCameraAdjustments.cpp
    ${UVCCAMERA}/src/ProgressObserver.cpp
    ${UVCCAMERA}/src/JniUVCCamera.cpp
    ${UVCCAMERA}/src/JpegBenchmark.cpp
    ${UVCCAMERA}/src/LoadJpegImageFromAssetsUseCase.cpp
    ${UVCCAMERA}/src/JniJpegBenchmark.cpp
    )
