cmake_minimum_required(VERSION 3.22)

if(BUILD_IMAGES_MODULE)
        add_library(Images STATIC
                src/ImageUseCases.cpp
                )

        target_include_directories(Images PUBLIC
                $<INSTALL_INTERFACE:include>
                $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                )                
endif()                