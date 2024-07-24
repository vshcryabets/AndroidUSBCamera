cmake_minimum_required(VERSION 3.10.2)

project("nativelib")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILATION_FLAGS} -std=c++17")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${COMPILATION_RELEASE_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${COMPILATION_DEBUG_FLAGS}")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMPILATION_FLAGS} -std=c99")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} ${COMPILATION_RELEASE_FLAGS}")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} ${COMPILATION_DEBUG_FLAGS}")

add_library(nativelib SHARED
        ${SRC_LAME}
        yuv.cpp
        proxy_yuv.cpp
        nativelib.cpp)

find_library(
        log-lib
        log)

target_link_libraries(
        nativelib
        ${log-lib})