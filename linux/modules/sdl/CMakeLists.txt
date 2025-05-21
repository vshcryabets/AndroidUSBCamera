cmake_minimum_required(VERSION 3.22)

if(BUILD_SDL_MODULE)
        find_package(SDL2 REQUIRED)

        add_library(SdlHelper STATIC
                src/SdlTools.cpp
                )

        target_include_directories(SdlHelper PUBLIC
                $<INSTALL_INTERFACE:include>
                $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                )                
endif()                