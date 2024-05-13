cmake_minimum_required(VERSION 3.10.2)

project("nativelib")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILATION_FLAGS} -std=c++17")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${COMPILATION_RELEASE_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${COMPILATION_DEBUG_FLAGS}")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMPILATION_FLAGS} -std=c99")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} ${COMPILATION_RELEASE_FLAGS}")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} ${COMPILATION_DEBUG_FLAGS}")

# lame
include_directories(${CMAKE_SOURCE_DIR}/module/mp3/lame)
aux_source_directory(${CMAKE_SOURCE_DIR}/module/mp3/lame SRC_LAME)

add_library(nativelib SHARED
        ${SRC_LAME}
        module/yuv/yuv.cpp
        module/mp3/mp3.cpp
        proxy/proxy_yuv.cpp
        proxy/proxy_mp3.cpp
        nativelib.cpp)

find_library(
        log-lib
        log)

target_link_libraries(
        nativelib
        ${log-lib})