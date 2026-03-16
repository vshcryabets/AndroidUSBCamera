cmake_minimum_required(VERSION 3.31)

add_library(common STATIC
        src/utilbase_jni.cpp
        )

target_include_directories(common PUBLIC
        $<INSTALL_INTERFACE:include>/include/
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
        )
