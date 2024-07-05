cmake_minimum_required(VERSION 3.22)

set(UVCCAMERA ${CMAKE_CURRENT_LIST_DIR}/)
include_directories(
    ${UVCCAMERA}/
)


message(${ANDROID_ABI})
if (ANDROID_ABI STREQUAL "arm64-v8a")
    add_compile_definitions(ABI=1)
elseif (ANDROID_ABI STREQUAL "armeabi-v7a")
    add_compile_definitions(ABI=2)
elseif (ANDROID_ABI STREQUAL "x86_64")
    add_compile_definitions(ABI=3)
elseif (ANDROID_ABI STREQUAL "x86")
    add_compile_definitions(ABI=4)
endif ()

list(APPEND SOURCES
	${UVCCAMERA}/_onload.cpp
    ${UVCCAMERA}/UVCCamera.cpp
    ${UVCCAMERA}/UVCPreviewBase.cpp
    ${UVCCAMERA}/UVCPreviewJni.cpp
    ${UVCCAMERA}/UVCCameraAdjustments.cpp
    ${UVCCAMERA}/JniUVCCamera.cpp
    )
