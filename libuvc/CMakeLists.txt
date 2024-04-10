cmake_minimum_required(VERSION 3.10.2)

project(libuvc)

include(./src/main/jni/libjpeg-turbo-1.5.0/android/CMakeLists.txt)

add_library(native SHARED ${SOURCES})

target_link_libraries(native
        android log)
