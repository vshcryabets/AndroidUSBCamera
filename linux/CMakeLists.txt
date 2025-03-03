cmake_minimum_required(VERSION 3.22)

project(LinuxUvcTests)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

find_package( Threads REQUIRED )
find_package (JPEG REQUIRED)

set(USE_TURBOJPEG 1)

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
        ../libausbc/src/main/jni/UVCCamera/JpegBenchmark.cpp
        /usr/include/turbojpeg.h
        )

if(USE_TURBOJPEG)
        target_compile_definitions(jpegbenchmark PRIVATE -DUSE_TURBOJPEG)
        add_library(turbojpeg SHARED IMPORTED)
        set_target_properties(turbojpeg PROPERTIES
                IMPORTED_LOCATION /usr/lib/libturbojpeg.so)
        target_link_libraries(
                jpegbenchmark
                turbojpeg
                ${CMAKE_THREAD_LIBS_INIT})         
endif()

if (USE_LIBJPEG)
        target_compile_definitions(jpegbenchmark PRIVATE -DUSE_LIBJPEG)
        target_link_libraries(
                jpegbenchmark
                ${JPEG_LIBRARIES}
                ${CMAKE_THREAD_LIBS_INIT})        
endif()


        
