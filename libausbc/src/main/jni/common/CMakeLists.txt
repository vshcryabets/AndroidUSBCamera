cmake_minimum_required(VERSION 3.22)

add_library(common STATIC
        utilbase.cpp
        )

target_include_directories(common PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
        )
