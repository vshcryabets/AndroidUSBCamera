cmake_minimum_required(VERSION 3.22)

list(APPEND SOURCES
    src/u8x8_fonts.c
)

if(BUILD_JNI_WRAPPERS)
    if (NOT (CMAKE_SYSTEM_NAME STREQUAL "Android"))
        find_package(JNI REQUIRED COMPONENTS JVM)
    endif()
    list(APPEND SOURCES src/FontWrapper.cpp)
endif()

add_library(U8x8Font STATIC
    ${SOURCES}
    )
set_target_properties(U8x8Font PROPERTIES POSITION_INDEPENDENT_CODE ON)
if(BUILD_JNI_WRAPPERS)
    target_include_directories(
        U8x8Font
        PUBLIC
        ${JNI_INCLUDE_DIRS}
        )
endif()

target_include_directories(U8x8Font PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )
