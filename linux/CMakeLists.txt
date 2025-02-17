cmake_minimum_required(VERSION 3.22)

project(LinuxUvcTests)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

find_package( Threads REQUIRED )

include_directories(
        ../libausbc/src/main/jni/UVCCamera/
)



add_executable(uvccapture
        uvccapture.cpp
)

target_link_libraries(
        uvccapture
        ${CMAKE_THREAD_LIBS_INIT})


add_executable(jpegbenchmark
        jpegdecodebenchmark.cpp
)

target_link_libraries(
        jpegbenchmark
        ${CMAKE_THREAD_LIBS_INIT})

        
