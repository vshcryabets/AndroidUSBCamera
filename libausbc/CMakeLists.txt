cmake_minimum_required(VERSION 3.22)

project(libausbc)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILATION_FLAGS} -std=c++17")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${COMPILATION_RELEASE_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${COMPILATION_DEBUG_FLAGS}")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMPILATION_FLAGS} -std=c99")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} ${COMPILATION_RELEASE_FLAGS}")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} ${COMPILATION_DEBUG_FLAGS}")

set(ANDROID_ARM_NEON TRUE)
set(REQUIRE_SIMD ON)
set(ENABLE_SHARED FALSE)
set(ENABLE_STATIC TRUE)

add_compile_definitions(LIBUVC_HAS_JPEG)

#include(../libausbc/src/main/jni/libjpeg-turbo-1.5.0/android/CMakeLists.txt)
include(../libausbc/src/main/jni/libjpeg-turbo-3.1.1-static/CMakeLists.txt)
include(../libausbc/src/main/jni/hacks/CMakeLists.txt)
include(../libausbc/src/main/jni/common/CMakeLists.txt)
include(../libausbc/src/main/jni/libusb/android/jni/CMakeLists.txt)
include(../libausbc/src/main/jni/libuvc/android/jni/CMakeLists.txt)
include(../libausbc/src/main/jni/UVCCamera/CMakeLists.txt)

add_library(native SHARED ${SOURCES})

target_link_libraries(native
        jpeg-static
        android
        log)
