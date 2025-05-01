cmake_minimum_required(VERSION 3.22)

project(LinuxUvcTests)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

find_package( Threads REQUIRED )
find_package (JPEG REQUIRED)

#set(USE_TURBOJPEG 1)
set(USE_LIBJPEG 1)

include_directories(
        ../libausbc/src/main/jni/UVCCamera/include/
        ./include/
)

find_package(SDL2 REQUIRED)

add_executable(uvccapture
        ./src/SdlTools.cpp
        ./src/UvcCamera.cpp
        ./src/uvccapture.cpp
        ../libausbc/src/main/jni/UVCCamera/src/ImageUseCases.cpp
        )

target_link_libraries(
        uvccapture
        ${SDL2_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT})


add_executable(jpegbenchmark
        ./src/jpegdecodebenchmark.cpp
        ../libausbc/src/main/jni/UVCCamera/src/ImageUseCases.cpp
        ../libausbc/src/main/jni/UVCCamera/src/JpegBenchmark.cpp
        ../libausbc/src/main/jni/UVCCamera/src/ProgressObserver.cpp
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

# add_executable(unix_socket_client unix_socket_client.cpp)
# add_executable(unix_socket_server unix_socket_server.cpp)
        
