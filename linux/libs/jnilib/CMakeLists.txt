cmake_minimum_required(VERSION 3.22)

if(BUILD_JNI_WRAPPERS)
    find_package(JNI REQUIRED COMPONENTS JVM)
    
    add_library(JniWrapper SHARED
        src/main.cpp
    )
    target_include_directories(
        JniWrapper
        PUBLIC
        ${JNI_INCLUDE_DIRS}
        )
    target_link_libraries(
        JniWrapper
        Sources
        Images
        U8x8Font
        JNI::JNI
        )
endif()