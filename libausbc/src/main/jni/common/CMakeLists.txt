cmake_minimum_required(VERSION 3.31)

add_library(common STATIC
        src/utilbase_cpp.cpp
)

target_include_directories(common PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        )