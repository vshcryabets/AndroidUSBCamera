cmake_minimum_required(VERSION 3.22)

project(libuvc)

#include(./src/main/jni/libjpeg-turbo-1.5.0/android/CMakeLists.txt)
#include(./src/main/jni/libjpeg-turbo-2.1.1/CMakeLists.txt)
add_subdirectory(./src/main/jni/libjpeg-turbo-2.1.1/)

include(./src/main/jni/common/CMakeLists.txt)
include(./src/main/jni/libusb/android/jni/CMakeLists.txt)
include(./src/main/jni/libuvc/android/jni/CMakeLists.txt)
include(./src/main/jni/UVCCamera/CMakeLists.txt)

add_library(native SHARED ${SOURCES})

target_link_libraries(native
        jpeg-static
        android
        log)
