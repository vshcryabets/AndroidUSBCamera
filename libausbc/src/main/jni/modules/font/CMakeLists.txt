cmake_minimum_required(VERSION 3.22)

list(APPEND SOURCES
        src/u8x8_fonts.c
)

if(BUILD_JNI_WRAPPERS)
        list(APPEND SOURCES src/FontWrapper.cpp)
endif()

add_library(U8x8Font STATIC
        ${SOURCES}
        )

target_include_directories(U8x8Font PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        )
