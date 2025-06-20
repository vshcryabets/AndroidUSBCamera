cmake_minimum_required(VERSION 3.22)

if(BUILD_U8X8FONT_MODULE)
        add_library(U8x8Font STATIC
                src/u8x8_fonts.c
                )

        target_include_directories(U8x8Font PUBLIC
                $<INSTALL_INTERFACE:include>
                $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                )
endif()