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
    # Force linker to report undefined symbols as errors during shared library creation
    if(APPLE)
        # On macOS/Apple platforms, use the ld64 linker syntax
        target_link_options(JniWrapper PRIVATE -Wl,-undefined,error)
    else()
        # On other platforms (like Linux), assume GNU ld linker syntax
        target_link_options(JniWrapper PRIVATE 
            -Wl,--no-undefined
            -Wl,--no-allow-shlib-undefined
        )
    endif()
endif()