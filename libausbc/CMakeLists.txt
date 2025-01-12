cmake_minimum_required(VERSION 3.22)

project(libuvc)

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

#set(LIBJPEG_PATH src/main/jni/libjpeg-turbo-2.1.1/)
#add_subdirectory(./${LIBJPEG_PATH})
#include_directories(./${LIBJPEG_PATH})
#include_directories(${CMAKE_CURRENT_BINARY_DIR}/${LIBJPEG_PATH})

add_compile_definitions(LIBUVC_HAS_JPEG)

#add_subdirectory(../libausbc/src/main/jni/libjpeg-turbo-2.1.1/)

include(../libausbc/src/main/jni/libjpeg-turbo-1.5.0/android/CMakeLists.txt)
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
