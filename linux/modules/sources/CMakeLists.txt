cmake_minimum_required(VERSION 3.22)

if(BUILD_SOURCES_MODULE)
        add_library(Sources STATIC
                src/Source.cpp
                src/TestSource.cpp
                src/UvcCamera.cpp
                )

        target_include_directories(Sources PUBLIC
                $<INSTALL_INTERFACE:include>
                $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                )                
endif()                