cmake_minimum_required(VERSION 3.31)

add_library(common INTERFACE)

target_include_directories(common INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/include/"
)
